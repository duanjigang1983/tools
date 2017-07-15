#ifndef _G_U_H_
#define _G_U_H_
#include "iniHelper.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <crypt.h>
#include <time.h>
#include <errno.h>
#include <curl/curl.h>

extern void usage(int argc, char* argv[]);
#define GK_STR_LEN 64
#define GK_FILE_LEN 128
#define GK_URL_LEN 128
enum	
	GK_ACTION 
{
		ACT_MIN_ACTION = 0,
		ACT_UPLOAD_PKG,
		ACT_LIST_REPO,
		ACT_LIST_PKG,
		ACT_MAX_ACTION
};
typedef struct _gk_conf_t
{
	enum GK_ACTION action;
    char conf[GK_FILE_LEN];
    char file[GK_FILE_LEN];
    char repo[GK_STR_LEN];
    char osv[GK_STR_LEN];
    char arch[GK_STR_LEN];
    char ak[GK_STR_LEN];
    char as[GK_STR_LEN];
    char uid[GK_STR_LEN];
    char url[GK_URL_LEN];
	char user[GK_STR_LEN];
}gk_conf_t;

typedef int (*gk_func_act_t)(gk_conf_t*);
typedef int (*gk_func_init_t)(int, char* [], gk_conf_t*);
typedef struct
{
	char * act_name;
	enum GK_ACTION act_id;
	gk_func_act_t 	act_func;
	gk_func_init_t 	init_func;
	char * act_desc;
}gk_action_t;

extern int an_args (int argc, char* argv[], gk_conf_t * gc);
extern int act_uploadpkg_init (int argc, char* argv[], gk_conf_t * gc);
extern int act_listrepo_init (int argc, char* argv[], gk_conf_t * gc);
extern int load_conf(gk_conf_t * gc);
extern int upload_pkg(gk_conf_t * gc);
extern int list_repo(gk_conf_t * gc);
extern char * randstr(int len);
#endif
