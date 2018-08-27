#include "gkutils.h"
#include "gk.h"
#include <getopt.h>

void usage(int argc, char* argv[])
{
    printf("usage:%s -c conf -k action [param1 param2 parms..]\n", argv[0]);
}

int an_args(int argc, char* argv[], gk_conf_t * gc)
{
	int opt,i, size;
	//struct stat st;
	char action[GK_STR_LEN] = {0};
    char *string = "hc:k:f:r:v:a:u:i:e:m:";
	memset(gc,0,sizeof(gk_conf_t));
	//printf("optind:%d，opterr：%d, optarg:%s, opt:%c\n",optind,opterr, optarg, opt);
    while ((opt = getopt(argc, argv, string))!= -1)
    {  
		switch(opt)
		{
			case 'c': //config file
					sprintf(gc->conf, optarg);
					break;	
			case 'k': //action name
					sprintf(action, optarg);
					break;	
			case 'e': //action name
					sprintf(gc->endpoint, optarg);
					break;	


			//default:
			case 'h':
				usage(argc, argv);
				return 1;
		}
    }  
	if(strlen(action) == 0)
	{
		usage(argc, argv);
		return 1;
	}
	#ifdef _DEBUG_
	printf("%s:%d:parese args over\n", __FILE__, __LINE__);
	#endif
	//check action
	gc->action = ACT_MIN_ACTION;
	size = sizeof(gk_action_list)/sizeof(gk_action_t);
	for(i = 0; i < size; i++)
	{
		#ifdef _DEBUG_
		printf("%d/%d-%s:%s\n", i, size,  gk_action_list[i].act_name, action);
		#endif
		if(strcmp(action, gk_action_list[i].act_name) == 0)
		{
			gc->action = i;		
		}
	}
	if((gc->action <= ACT_MIN_ACTION) || (gc->action >= ACT_MAX_ACTION))
	{
		printf("Invalid action name:'%s',only support:\n", action);
		//for(i=0;i < size; i++)
		for(i=1;i < size-1; i++)
		{
			printf("\t\"%s\":\t\t\"%s\"\n", gk_action_list[i].act_name, gk_action_list[i].act_desc);
		}
		return 1;
	}
	//then check
	if(strlen(gc->conf) == 0) 
	{
		if(access(default_conf, R_OK))
		{
			printf("conf is needed\n");
			usage(argc, argv);
			return 1;
		}
		strcpy(gc->conf, default_conf);
	}

	// check file
	//if(stat(gc->conf, &st)) {printf("can not access file: '%s'\n",gc->conf);return 1;}		
	#ifdef _DEBUG_
	printf("%s:%d:parese args success,action=%d\n", __FILE__, __LINE__,gc->action);
	#endif

	return 0;
}

int load_conf(gk_conf_t * gc)
{
	void * ini = NULL;
    //section sc;
    if(NULL == (ini = init_ini(gc->conf)))
    {
            printf ("open file '%s' for reading failed\n", gc->conf);
            return 1;
    }
	sprintf (gc->ak, "%s", read_string(ini, "config", "access_key", "none"));	
	sprintf (gc->as, "%s", read_string(ini, "config", "access_secret", "none"));	
	sprintf (gc->uid, "%s", read_string(ini, "config", "uid", "none"));	
	sprintf (gc->uid, "%s", read_string(ini, "config", "uid", "none"));	
	sprintf (gc->output, "%s", read_string(ini, "system", "output", "text"));	
	sprintf (gc->url, "%s", read_string(ini, "system", "gkhome", "ease2cloud.com/easepkg"));	
	sprintf (gc->upload_method, "%s", read_string(ini, "system", "upload", "gkupload.php"));	

	//firstly, we accept input from the user
	
	if(strlen(gc->endpoint) > 0)
	{
		memset(gc->url, 0, sizeof(gc->url));
		strcpy(gc->url, gc->endpoint);
	}
	//check ak
	if(NULL != strstr(gc->ak, "none"))
	 {
		printf("can not read 'access_key' from section '[config]'\n"); 
		close_ini(ini); 
		return 1;
	}	
	//check as
	if(NULL != strstr(gc->as, "none"))
	 {
		printf("can not read 'access_secret' from section '[config]'\n"); 
		close_ini(ini); 
		return 1;
	}	
	//check uid
	if(NULL != strstr(gc->uid, "none")) 
	{
		printf("can not read 'uid' from section '[config]'\n"); 
		close_ini(ini); 
		return 1;
	}
	//check by server but not client	
	/*if((strcmp(gc->output, "text"))&&(strcmp(gc->output, "json")))
	{
		printf("output in file '%s' can only be text or json\n", gc->conf); 
		close_ini(ini); 
		return 1;

	}*/
	close_ini(ini);
	//printf("load config success:%s,%s,%s,%s\n", gc->ak, gc->as, gc->uid, gc->url);
	return 0;

}

char * randstr(int len)
{
	static char ret[128]= {0};
	memset(ret, 0, 128);
	int i = 0;
	int nlen = len;
	if(nlen > 128) nlen = 128;
	for(i=0; i < nlen; i++)
	{
		char ch;
		int a = rand()%26;
		if(i%3==0) ch = 'A';
		else ch = 'a';
		ch = ch + a;
		ret[i] = ch;
	}	
	return ret;
}

