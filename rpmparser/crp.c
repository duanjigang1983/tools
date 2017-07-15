////////// gcc test.c -I/usr/include/rpm -lrpm -lrpmdb -lrpmio -lpopt -o test_rpm
//require rpm-devel
//example by cme@ 2012-11-16

#include <rpmlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <header.h>
#include <rpmio.h>
#include <popt.h>
#include <errno.h>
#include "datadef.h"
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>


int samplerpm (const char *szrpm);
char * readHeaderString (Header hd, int tag_id);
int main (int argc, char* argv[])
{
	if (argc > 0)
	{
		int i = 0;
		for (i = 1; i < argc; i++)
		{
			if(samplerpm (argv[i])) return 1;
		}
	}
	return 0;	
}

char * readHeaderString (Header header, int tag_id)
{
  	int type;
  	void *pointer;
  	int data_size;
  	int header_status = headerGetEntry (header, tag_id, &type, &pointer, &data_size);
  	if (header_status)
    	{
      		if (type == RPM_STRING_TYPE)
		{
	  		return pointer;
		}
    	}
  	return NULL;
}

int samplerpm (const char *szrpm)
{
	char g_szname[1024] = {0};
	FD_t fd = Fopen (szrpm, "r");
	int nfd = 0;
	size_t st = 0;
	memset (g_szname, 0, 1024);
	sprintf (g_szname, "%s", szrpm);
	fflush (stdin);
	fflush (stdout);
  	if (!fd)
    	{
      		return 1;
    	}
  	 
  	struct rpmlead plead;
	nfd = open(szrpm, O_RDONLY);
	if(nfd < 0)
	{
		 printf("open %s failed\n", szrpm);
		 return 1;
	}
	st = read(nfd, &plead, sizeof(struct rpmlead));
	if(st <= 0)
	{
		printf("read error\n");
		close(nfd);
		return 1;
	}
	close(nfd);
	printf("%s, %d\n", plead.name, plead.major);
	rpmRC rc = RPMRC_FAIL; 

	 const char item[] = "Signature";
	const char* msg  = NULL;
	rc = rdLead(fd, NULL, msg);
	 //rpmpkgRead(item, fd, NULL, &msg);
	 rc = rdSignature(fd, NULL, &msg);


  	Header header;
  	rpmRC ret = rpmReadSignature (fd, &header, plead.signature_type);

  	if (ret != RPMRC_OK)
    	{
      		printf ("rpmReadSignature of '%s' failed\n", szrpm);
      		Fclose (fd);
      		return 1;
    	}
  	else
  	printf ("rpmReadSignature success:%s\n", szrpm);
  	//read header
    //rpmReadSignature(fd, NULL, plead.signature_type, NULL);
  	Header newheader = headerRead (fd, (plead.major >= 3) ? HEADER_MAGIC_YES : HEADER_MAGIC_NO);
  	//Header newheader = headerRead (fd, (5 >= 3) ? HEADER_MAGIC_YES : HEADER_MAGIC_NO);
  	if (!newheader)
    	{
      		//printf ("headerRead of '%s' failed\n", szrpm);
      		Fclose (fd);
      		return 1;
   	 }
	const char *name = readHeaderString (newheader, RPMTAG_NAME);
  	const char *version = readHeaderString (newheader, RPMTAG_VERSION);
  	const char *release = readHeaderString (newheader, RPMTAG_RELEASE);
  	const char *group = readHeaderString (newheader, RPMTAG_GROUP);
  	const char *packager = readHeaderString (newheader, RPMTAG_PACKAGER);

	if (!group) group = "NONE_GROUP";
	if (!packager) packager = "NONE_PACKAGER";
	printf ("name:%s\nversion:%s\nrelease:%s\ngroup:%s\npackager:%s\n", 
	name, version, release, group, packager);
  	Fclose (fd);
  	return 0;
}
