///by duanjigang1983 <2013-11-08>
#include <stdlib.h>
#include <stdio.h>
#include "colordefine.h"
#include "inihelper.h"

int main (int argc, char * argv[])
{
	void * ini = NULL;
	const char* conf = "ex.ini";
	section sc;
	if(NULL == (ini = init_ini(conf)))
     {
			printf ("%s-%d:open file '%s' for reading failed\n",__FILE__, __LINE__, conf);
            return 1;
     }
	char sigfile[256] = {0};
	int interval = atoi (read_string(ini, "general", "interval", "600"));
	int nthread = atoi (read_string(ini, "general", "thread_num", "10"));
	sprintf (sigfile, "%s", read_string(ini, "general", "signalfile", "update.txt"));

	printf ("\n\tinterval = "YELLOW"%d"NONE"\n\tthread_num = "YELLOW"%d"NONE"\n\tsignalfile="YELLOW"%s"NONE"\n",  interval, nthread, sigfile);
	//reading each repo
	
	while (read_section(ini, &sc))
    {   
        int i = 0;
		char path[256] = {0};
		int interval = 0;
		int on = 0;
		if (strncmp(sc.title, "repo", 4))continue;
	
        for ( i = 0; i < sc.entry_number; i++)
           {
                  if (strncasecmp(sc.entry_list[i].var_name, "path", 4) == 0)
                    {
                           strcpy(path, sc.entry_list[i].var_value);
                           continue;
                     }	
				  
				  	if (strncasecmp(sc.entry_list[i].var_name, "on", 2) == 0)
                    {
                           on = atoi(sc.entry_list[i].var_value);
                           continue;
                     }	
					
					if (strncasecmp(sc.entry_list[i].var_name, "interval", 8) == 0)
                    {
                           interval = atoi(sc.entry_list[i].var_value);
                           continue;
                     }
		}
		printf ("path="GREEN"%s"NONE", interval="GREEN"%d"NONE", on="GREEN"%s"NONE"\n", path,interval,(on==1)?"yes":"no");
	}
	close_ini (ini);
}

