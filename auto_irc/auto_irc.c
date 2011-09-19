#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
//:ray!~rayren@10.32.101.216 KICK #jumbo wind :ray
const char*  USER =  "wind";
const char*  NICK =  "wind";
#define DATA_DIR "/tmp/irc"
const char* CHANNEL =  "#TAOBAO";
char clist[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

extern int tcp_send(int aFd,char *aBuf,int aLen);
extern int tcp_read(int aFd,char *aBuf);
extern int init_tcp_sock(int	aMode, const char *aAddr, unsigned short  aPort);
extern int tcp_connect(int fd, const char *aAddr, int aPort);
int post_msg(int sock);
int handle_msg (const char* szbuf, int sock, int id);
int post_heartbt_msg(int sock, int index);

void * irc_thread(void * data);
int bit[100];
pthread_t tid[100];
time_t tmlist[100];
static int kick = 0;
static char g_killer[64] = {0};
int irc(int * data);
int main(int argc, char* argv[])
{
	int n = 4;
	int i = 0;
		
	srand(time(0));
	if (argc > 1) n = atoi(argv[1]);
	if ((n == 0) || (n > 100)) n = 4;
	for (i = 0; i< 100; i++)
	{
		 bit[i] = 0;
		tmlist[i] = time(0);
	}

	n = 1;	
		for (i = 0; i < n; i++)
		{
		
			if (bit[i] == 0)
			{
				bit[i] = i + 1;
				/*if (pthread_create (tid + i, 0, irc_thread, bit + i))
				{
					printf ("create thread %d failed\n", i);
				}else
				{
					printf ("robot %d running\n", i + 1);
				}*/	
			}
			//sleep(rand()%20+10);
		}
		//sleep(5);
	//*/
	irc(&n);	
	return 0;
}

void * irc_thread(void * data)
{
	return 0;
}
int irc(int * data)
{
	int *index = (int*)data;
	
	const char * host = "opsirc.corp.taobao.com";
	char szbuf[8192] = {0};
	unsigned short port = 6667;
	int mode = 'c';

	int sock = init_tcp_sock (mode, host, port);
	//pthread_detach (pthread_self());
	if (sock <= 0)
	{
		printf ("init socket failed\n");
		goto ret;
	}
	
	if (tcp_connect(sock, host, port))
	{
		printf("connect host '%s:%u' failed\n", host, port);
		close(sock);
		goto ret;
	}

	while (1)
	{
		int len = 0;
		memset (szbuf, 0, strlen(szbuf));
		len =  tcp_read(sock, szbuf);
		if (len < 0)
		{
			close(sock);
			break;
		}
		
		if ((len == 0)&&(kick))
		{
			memset (szbuf, 0, sizeof(szbuf));
			sprintf (szbuf, "%s %s", "376", "End");
			len = strlen(szbuf);
			kick = 0;
			//printf ("killer:%s,kick:%d\n", g_killer, kick);
			sleep(3);
		}
		if (len == 0)
		{
			int nret = 0;
			usleep(100);
			nret = post_msg(sock);
			if (nret < 0)
			{
				break;
			}
			if (nret == 0)
			{
				if (post_heartbt_msg(sock, *index) < 0)
				{
					break;
				}
				usleep(100);
			}
			
			continue;
		}
		if (len > 0)
		{
			if (!strlen(g_killer))
			printf ("%s", szbuf);

			handle_msg(szbuf, sock, *index);
		}
	}
ret:
	*index = 0;
	//pthread_exit (0);
	return 0;
}


int handle_msg (const char* szbuf, int sock, int id)
{
	//(1) login
	char * p = 0;
	char * q = 0;
	char reply[1024] = {0};
	int have_data = 0;
	//login 
	if (!have_data)
	{
		p = strstr (szbuf, "Found your hostname");
		if (p)
		{
			//int int1 = rand()%26;
			//int int2 = rand()%26;
			sprintf (reply, "USER %s 1 2 3\nNICK %s 1 2 3\n", USER, NICK);
			/*sprintf (reply, "USER %c%c%d 1 2 3\nNICK %c%c%d 1 2 3\n", 
			clist[int1], clist[int2], id,  
			clist[int1], clist[int2], id);	
			*/
			have_data = 1;
		}
	}
	//login success		
	if (!have_data)
	{
		p = strstr(szbuf, "376");
		q = strstr (szbuf, "End");
		if (q || p)
		{
			sprintf (reply,  "join %s\n", CHANNEL);
			have_data = 1;
		}
	}
	//Ping message
	if (!have_data)
	{
		p = strstr(szbuf, "PING");
		if (p)
		{
			sprintf (reply, "PONG %s\n",  p + 4);
			have_data = 1;
		}
	}
	//kick sombody
	//:ray!~rayren@10.32.101.216 KICK #jumbo wind :ray
	if (!have_data)
	{
		p = strstr(szbuf, "was kicked from");
		char name[64] = {0};
		char killer[64] = {0};
		if (p)
		{
			char * q = p-1;
			while(isspace(*q) && (q != szbuf)) q--;
			if (q==szbuf) return 1;
			p = q;
			while((q != szbuf) && !isspace(*q))q--;
			if (q == szbuf) return 1;
			q++;
			strncpy(name, q, p-q+1);
		}
		if (p)
		{
			char * q = strstr(szbuf, "by");
			if (q)
			{
				char * s = 0;
				q += 3;
				s = q;
				while(s && !isspace(*s) && (*s != '\n'))s++;
				if (s)
				{
					strncpy(killer, q, s - q);
				}
				
			}
		}
		if (strlen(name) > 0)
		{
			if (strlen(killer) == 0) sprintf (killer, "%s", "sombody");
			sprintf (reply, "poor %s was kicked by %s, haha, %s is a bad boy....\r\n",
			name,  killer, killer);
			have_data = 1;
		}
	}	

	//myself is kicked
	if (!have_data)
	{
		//:ray!~rayren@10.32.101.216 KICK #jumbo wind :ray
		p  = strstr(szbuf, "KICK");
		//char * q = strstr(szbuf, USER);
		if (p)
		{
			 q = (char*)(szbuf + strlen(szbuf) - 1);
			 while(*q != ':')
			{
				q--;
			}
			if (*q == ':')
			{
				memset (g_killer, 0, sizeof(g_killer));
				sprintf (g_killer, "%s", q+1);
				g_killer[strlen(g_killer) - 2] = '\0';
				kick = 1;
				printf ("killer is :(%s)\n", g_killer);
			}	
		}
	}
	
	if (have_data)
	{
		if (tcp_send(sock, reply, strlen(reply)) < 0 )
		{
			printf ("sending:'%s' failed\n", reply);
			sleep(2);
			return -1;
		}else
		{
			//printf ("sending '%s' success\n", reply);
		}
	}
	return 1;
}
int post_msg(int sock)
{
	DIR * dir;
        struct dirent* drt;
	int have_data = 0;
	if (strlen(g_killer) > 0)
	{
		char szline[256];
		char msg[128] = {0};
		//sprintf (msg, " %s shit,why-did-%s-kick-me,he-must-be-a-bad-boy..\n", CHANNEL, g_killer);
		sprintf (msg, " %s shit, why did %s kick me, he must be a bad boy..\n", CHANNEL, g_killer);
		sprintf (szline, "PRIVMSG %s\n", msg);		
		printf ("==%d==%s", strlen(szline), szline);
		int len = strlen(szline);
		while(len > 0)
		{	
			int send = tcp_send(sock, szline, strlen(szline));
			if (send < 0)
			{
				have_data = -1;
				break;
			}
			else 
			{
				len -= send;
				have_data = 1;
			}
			usleep(10);
		}
		memset (g_killer, 0, sizeof(g_killer));
		return have_data;
	}
        dir = opendir (DATA_DIR);
        if (NULL == dir)
        {
                printf ("%s-%d:open dir %s failed\n", __FILE__, __LINE__, DATA_DIR);
               	sleep (5);
		return 0;
        }
        //read each subdir and file
        while ((drt = readdir(dir)) != NULL)
        {
                char szbuf[1024] = {0};
                char name[128] = {0};
		FILE * fp = 0;
                if ((strcmp(drt->d_name, ".")==0)|| (strcmp(drt->d_name, "..")==0))
                {
                        //ignore current dir and parent dir pointer
                        continue;
                }
		sprintf (name, "%s/%s", DATA_DIR, drt->d_name);
		printf ("open file %s\n", name);
		fp = fopen (name, "r");
		if (!fp)
		{
			printf ("open file '%s' failed\n", name);
			continue;
		}
		sprintf (szbuf, "PRIVMSG %s ", CHANNEL);
		while (fgets(szbuf+strlen(szbuf), 1024, fp))
		{	
			strcat(szbuf, "\n");
			have_data = 1;
			if (tcp_send(sock, szbuf, strlen(szbuf)) < 0)
			{
				have_data = -1;
				break;
			}else
			{
				printf ("sending:%s\n", szbuf);
			}
		}
		fclose(fp);
		unlink(name);
		sleep(1);
	}
	closedir (dir);
	return have_data;
}

int post_heartbt_msg(int sock , int index)
{
	time_t tm = time(0);
	if (tm - tmlist[index] >= 30)
	{
		tmlist[index]= tm;	
		if (tcp_send(sock, "nothing", 5) < 0 )
		{
			return -1;
		}
	}	
	return 1;
}
