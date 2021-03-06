#ifndef _GK_H_
#define _GK_H_
#include "gkutils.h"

static gk_action_t gk_action_list [] = 
{
	{"min_act", 	ACT_MIN_ACTION, 	NULL, 			"never define anything before this item"},
	{"uploadpkg", 	ACT_UPLOAD_PKG, 	&upload_pkg, 	"upload a package to a valid repo"},
	{"listrepo", 	ACT_LIST_REPO, 		NULL, 			"list all repos you can access"},
	{"listpkg", 	ACT_LIST_PKG,		NULL, 			"list all packages you can access"},
	{"max_act", 	ACT_MAX_ACTION,		NULL, 			"never define anything after this item"}
};
#endif
