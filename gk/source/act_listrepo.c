#include "gkutils.h"
static void my_usage(int argc, char* argv[])
{
    printf("usage:%s -c conf -k listrepo -u [me/any]\n", argv[0]);
}

int act_listrepo_init(int argc, char* argv[], gk_conf_t * gc)
{
	int opt;
	struct stat st;
	//char action[GK_STR_LEN] = {0};
    char *string = "hc:k:u:";
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
			case 'u': //user
					if((strcmp(optarg,"me"))&&(strcmp(optarg, "any")))
					{ 
						my_usage(argc, argv);
						return 1;
					}
					sprintf(gc->user, optarg);
					break;
			/*	
			case 'r': //repo name
					sprintf(gc->repo, optarg);
					break;	
			case 'v': //os version
					sprintf(gc->osv, optarg);
					break;	
			case 'a': //arch number
					sprintf(gc->arch, optarg);
					break;	
			*/
			//default:
			case 'h':
				my_usage(argc, argv);
				return 1;
		}
    }  
	#ifdef _DEBUG_
	printf("%s:%d:parese args over\n", __FILE__, __LINE__);
	#endif
	//if(strlen(gc->file) == 0) {printf("uploadpkg file is needed\n"); my_usage(argc, argv);return 1;}
	//if(strlen(gc->repo) == 0) {printf("repo is needed\n"); my_usage(argc, argv);return 1;}
	//if(strlen(gc->osv) == 0) {printf("version is needed\n"); my_usage(argc, argv);return 1;}
	if(strlen(gc->user) == 0) //{printf("arch is needed\n"); my_usage(argc, argv);return 1;}
	{
		sprintf(gc->user, "%s", "any");
	}
	// check file
	if(stat(gc->conf, &st)) {printf("can not access file: '%s'\n",gc->conf);return 1;}		
	//if(stat(gc->file, &st)) {printf("can not access file: '%s'\n",gc->file);return 1;}		
	#ifdef _DEBUG_
	printf("%s:%d:parese args success,conf=%s,user=%s\n", __FILE__, __LINE__, gc->conf, gc->user);
	#endif
	return 0;
}


int list_repo(gk_conf_t * gc)
{
	CURL *curl;
    //int still_running;
    //struct curl_httppost *formpost = NULL;
    //static const char buf[] = "Expect:";
    char strurl[512] = {0};
    char  ak[128] = {0};
    //struct stat st; 
    time_t s_tm;
	char numran[10] = {0};
	char strran[80] = {0};
	char strts[30] = {0};
	int pos1 =0, pos2 = 0;
	char salt[2] = {0};
	//printf("list repo over...\n");
	//return 0;
	//char strdata[60] = {0};
    time(&s_tm);
 	sprintf(numran, "%s", randstr(10));
	sprintf(strts, "%u%s", (unsigned int)s_tm, numran);
	pos1 = strts[8] - '0';
	pos2 = strts[9] - '0';
	salt[0] = numran[pos1];
	salt[1] = numran[pos2]; 
	strncpy(strran, crypt(gc->as, salt), 80);
    //sprintf(strurl, "http://%s/gk_listrepo.php?uid=%s&token=%s", gc->url, gc->uid, strts);
    sprintf(strurl, "http://%s/gk_listrepo.php?", gc->url);
	#ifdef _DEBUG_
	printf("%s:%d:url=%s\n", __FILE__, __LINE__, strurl);
	#endif
	sprintf(ak, "%s:%s", gc->ak, strran);
  	#ifdef _DEBUG_
	printf("%s:%d:ak=[%s]\n", __FILE__, __LINE__, ak);
	#endif
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init ();
    if (curl)
    {
		CURLcode res;
		char data[256] = {0};
		sprintf(data, "uid=%s&token=%s&out=%s&user=%s", gc->uid, strts, gc->output, gc->user);
		//sprintf(data, "uid=%s&token=%s", gc->uid, strts);
        curl_easy_setopt (curl, 	CURLOPT_URL, strurl);
        curl_easy_setopt (curl, 	CURLOPT_VERBOSE, 0L); //1 to show 0 for silence
        curl_easy_setopt (curl, 	CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, 		CURLOPT_USERPWD, ak);
    	#ifdef _DEBUG_
		printf("%s:%d:postdata=[%s],ak=%s\n", __FILE__, __LINE__, data, ak);
		#endif
		res = curl_easy_perform(curl);
		// Check for errors 
    	if(res != CURLE_OK)
      		fprintf(stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror(res));
        //time(&now);
        curl_easy_cleanup (curl);
  	 }else printf("curl init failed\n");
	curl_global_cleanup();
	return 0;
}

