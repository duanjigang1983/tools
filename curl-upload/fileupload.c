/* This is an example application source code using the multi interface 
* to do a multipart formpost without "blocking". */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <curl/curl.h>

int main (int argc, char *argv[])
{
    CURL *curl;

    CURLM *multi_handle;
    int still_running;
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;
    struct curl_slist *headerlist = NULL;
    static const char buf[] = "Expect:";
    const char* uid="fEWREWREWREWREW";
    char strurl[512] = {0};
    struct stat st; 
    time_t s_tm,e_tm, now;
    char * fname, * url;

    if(argc < 2){
        printf("usage:%s file url\n", argv[0]);
        return 0;
    }
  
    fname = argv[1];
    url = argv[2];
    if(stat(fname, &st)){
        printf("can not access file:%s\n", fname);
        return 0;  
    }
    sprintf(strurl, "%s?uid=%s", url, uid);
    time(&s_tm);
  
    /* Fill in the file upload field. This makes libcurl load data from 
     the given file name when curl_easy_perform() is called. */
    curl_formadd (&formpost, &lastptr, CURLFORM_COPYNAME, "gingko-uploader", CURLFORM_FILE, fname, CURLFORM_END);
    
    /* Fill in the filename field */
    curl_formadd (&formpost, &lastptr, CURLFORM_COPYNAME, "filename", CURLFORM_COPYCONTENTS, fname, CURLFORM_END);

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
        curl_easy_setopt(curl, CURLOPT_USERPWD, "cmesoft:password");

        curl_multi_perform (multi_handle, &still_running);
        printf("start uploading now...\n");
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

        printf("\n");
        curl_multi_cleanup (multi_handle);

        /* always cleanup */
        curl_easy_cleanup (curl);

        /* then cleanup the formpost chain */
        curl_formfree (formpost);

      /* free slist */
        curl_slist_free_all (headerlist);
        }

        time(&e_tm);
        if(1){
        int ts_df = e_tm - s_tm;
        off_t size = (st.st_size);
        if(ts_df <=0) ts_df = 1;
        printf("Finished uploading '%s' to '%s'\n", fname, url);
        printf("%d second(s) elapsed, size: %u Kb,  avg speed: %u KB/s\n", ts_df, size, size/ts_df);
    }  
    return 0;
}
