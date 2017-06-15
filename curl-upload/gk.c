/* This is an example application source code using the multi interface 
* to do a multipart formpost without "blocking". */

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <curl/curl.h>
#include "gkutils.h"
#include "gk.h"

int main (int argc, char *argv[])
{
	gk_conf_t gc;
	char strurl[512] = {0};
    struct stat st; 
    time_t s_tm,e_tm, now;
    char * fname, * url;
	srand((unsigned) time(NULL));
	if(an_args(argc, argv, &gc))
	{
		 return 0;
	}
	if(load_conf(&gc))
	{
		 return 0;
	}
	if (gk_action_list[gc.action].act_func)
	{
		return gk_action_list[gc.action].act_func(&gc);	
	}
    return 0;
}
