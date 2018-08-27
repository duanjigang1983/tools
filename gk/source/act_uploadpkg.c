#include "gk.h"
static void my_usage(int argc, char* argv[])
{
    printf("usage:%s -c conf -k action -f rpm/deb-file -v os -r reponame -i instance -m information \n", argv[0]);
}

int act_uploadpkg_init(int argc, char* argv[], gk_conf_t * gc)
{
	int opt;
	struct stat st;
	//char action[GK_STR_LEN] = {0};
    char *string = "hc:k:f:v:r:a:i:e:m:";
	optind = 1;
	//opt='\0';
    while ((opt = getopt(argc, argv, string))!= -1)
    {  
		//printf("-->%c\n", opt);
		//if(optarg) printf("==>%s\n", optarg);
		switch(opt)
		{
			/*
			case 'c': //config file
					sprintf(gc->conf, optarg);
					break;	
			case 'k': //action name
					sprintf(action, optarg);
					break;	
			*/
			case 'f': //upload file
					sprintf(gc->file, optarg);
					break;	
			case 'r': //repo name
					sprintf(gc->repo, optarg);
					break;	
			case 'v': //os version
					sprintf(gc->osv, optarg);
					break;	
			case 'i': //arch number
					sprintf(gc->app, optarg);
					break;	
			case 'm': //arch number
					sprintf(gc->info, optarg);
					break;	

			//default:
			case 'h':
				my_usage(argc, argv);
				return 1;
		}
    }  
	#ifdef _DEBUG_
	printf("%s:%d:parese args over\n", __FILE__, __LINE__);
	#endif
	if(strlen(gc->file) == 0) {printf("uploadpkg file is needed\n"); my_usage(argc, argv);return 1;}
	if(strlen(gc->repo) == 0) {printf("repo is needed\n"); my_usage(argc, argv);return 1;}
	if(strlen(gc->osv) == 0) {printf("version is needed\n"); my_usage(argc, argv);return 1;}
	if(strlen(gc->app) == 0) {printf("instance name is needed\n"); my_usage(argc, argv);return 1;}
	// check file
	//if(stat(gc->conf, &st)) {printf("can not access file: '%s'\n",gc->conf);return 1;}		
	if(stat(gc->file, &st)) {printf("can not access file: '%s'\n",gc->file);return 1;}		
	#ifdef _DEBUG_
	printf("%s:%d:parese args success,file=%s,repo=%s,osv=%s\n", __FILE__, __LINE__, gc->file, gc->repo, gc->osv);
	#endif
	return 0;
}


int upload_pkg(gk_conf_t * gc)
{
	CURL *curl;
    CURLM *multi_handle;
    int still_running;
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
    static const char buf[] = "Expect:";
    char strurl[512] = {0};
    char  ak[128] = {0};
    //struct stat st; 
    time_t s_tm,e_tm, now;
	char numran[10] = {0};
	char strran[80] = {0};
	char strts[30] = {0};
	int pos1 =0, pos2 = 0;
	char salt[2] = {0};
	//char strdata[60] = {0};
    time(&s_tm);
 	sprintf(numran, "%s", randstr(10));
	sprintf(strts, "%u%s", (unsigned int)s_tm, numran);
	pos1 = strts[8] - '0';
	pos2 = strts[9] - '0';
	salt[0] = numran[pos1];
	salt[1] = numran[pos2]; 
	//sprintf(strdata, "%s%s", gc->ak, strts);
	strncpy(strran, crypt(gc->as, salt), 80);
//	printf("strts=%s,straran=%s,salt=%s(%d-%d)\n", strts, strran, salt, pos1, pos2);
	
	//return 0; 
    //sprintf(strurl, "http://%s/gkupload.php?uid=%s&repo=%s&osv=%s&arch=%s&token=%s", gc->url, gc->uid, gc->repo, gc->osv, gc->arch, strts);
    if(strlen(gc->info) > 0)
    sprintf(strurl, "http://%s/%s?uid=%s&repo=%s&osv=%s&token=%s&app=%s&info=%s", gc->url, gc->upload_method,  gc->uid, gc->repo, gc->osv,  strts, gc->app, gc->info);
	else
    sprintf(strurl, "http://%s/%s?uid=%s&repo=%s&osv=%s&token=%s&app=%s", gc->url, gc->upload_method,  gc->uid, gc->repo, gc->osv,  strts, gc->app);
	#ifdef _DEBUG_
	printf("%s:%d:url=%s\n", __FILE__, __LINE__, strurl);
	#endif
	sprintf(ak, "%s:%s", gc->ak, strran);
  	#ifdef _DEBUG_
	printf("%s:%d:ak=[%s]\n", __FILE__, __LINE__, ak);
	#endif

    /* Fill in the file upload field. This makes libcurl load data from 
     the given file name when curl_easy_perform() is called. */
    curl_formadd (&formpost, &lastptr, CURLFORM_COPYNAME, "gingko-uploader", CURLFORM_FILE, gc->file, CURLFORM_END);
    
    /* Fill in the filename field */
    curl_formadd (&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, gc->file, CURLFORM_END);

    /* Fill in the submit field too, even if this is rarely needed */

    curl_formadd (&formpost, &lastptr, CURLFORM_COPYNAME, "submit", CURLFORM_COPYCONTENTS, "send", CURLFORM_END);

	curl = curl_easy_init ();
    multi_handle = curl_multi_init ();

    /* initalize custom header list (stating that Expect: 100-continue is not 
     wanted */
    headerlist = curl_slist_append (headerlist, buf);
    if (curl && multi_handle)
    {
		/* what URL that receives this POST */
        curl_easy_setopt (curl, CURLOPT_URL, strurl);
        curl_easy_setopt (curl, CURLOPT_VERBOSE, 0L); //1 to show 0 for silence

        curl_easy_setopt (curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt (curl, CURLOPT_HTTPPOST, formpost);

        curl_multi_add_handle (multi_handle, curl);

        //curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY);
 
        /* set user name and password for the authentication */ 
        curl_easy_setopt(curl, CURLOPT_USERPWD, ak);

        curl_multi_perform (multi_handle, &still_running);
        //printf("start uploading now...\n");
        time(&now);
        do
	    {
	        struct timeval timeout;
	        int rc;		/* select() return code */
            time_t ntm;
	        fd_set fdread;
	        fd_set fdwrite;
	        fd_set fdexcep;
	        int maxfd = -1;

	        long curl_timeo = -1;
            
	        FD_ZERO (&fdread);
	        FD_ZERO (&fdwrite);
	        FD_ZERO (&fdexcep);

	        /* set a suitable timeout to play around with */
	        timeout.tv_sec = 1;
	        timeout.tv_usec = 0;

	        curl_multi_timeout (multi_handle, &curl_timeo);
	        if (curl_timeo >= 0)
	        {
	            timeout.tv_sec = curl_timeo / 1000;
	            if (timeout.tv_sec > 1)
		        timeout.tv_sec = 1;
	            else
		        timeout.tv_usec = (curl_timeo % 1000) * 1000;
	        }

	    /* get file descriptors from the transfers */
	        curl_multi_fdset (multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
			/* In a real-world program you OF COURSE check the return code of the 
	     function calls.  On success, the value of maxfd is guaranteed to be 
	     greater or equal than -1.  We call select(maxfd + 1, ...), specially in 
	     case of (maxfd == -1), we call select(0, ...), which is basically equal 
	     to sleep. */

            rc = select (maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
	        switch (rc)
            {
	            case -1:
                    printf("Error.........\n");
                    return 1;
	             /* select error */
	                break;
	            case 0:
	            default:
	            /* timeout or readable/writable sockets */
	            //printf ("#");
	                curl_multi_perform (multi_handle, &still_running);
	                //printf ("running: %d!\n", still_running);
                    time(&ntm);
                    if(ntm - now >= 1)
                    {
                        now = ntm;
                        //printf("#");
                    }
	                break;
	        }
        }while (still_running);

        //printf("\n");
        curl_multi_cleanup (multi_handle);

        /* always cleanup */
        curl_easy_cleanup (curl);

        /* then cleanup the formpost chain */
        curl_formfree (formpost);

      /* free slist */
        curl_slist_free_all (headerlist);
        }

        time(&e_tm);
      	if(1)
		{
        	int ts_df = e_tm - s_tm;
        	//off_t size = (st.st_size);
        	if(ts_df <=0) ts_df = 1;
        	//printf("Finished uploading '%s' to '%s'\n", gc->file, strurl);
        	//printf("%d second(s) elapsed, size: %u Kb,  avg speed: %u KB/s\n", ts_df, size, size/ts_df);
		}
		return 0;
}

