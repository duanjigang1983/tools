//created by duanjigang1983 @2014-01-24 for hprint rpm file
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char* argv[])
{
	if ( argc != 2 )
	{
		printf("usage:%s filename\n", argv[0]);
		return 1;
	}
	char* fname = argv[1];
	int fd = open (fname, O_RDONLY);
	if (fd == -1)
	{
		printf ("open file %s for read failed\n", fname);
		return 1;
	}
	char szline[16] = {0};
	unsigned int pos = 0;
	unsigned int line = 0;
	int nread = 0;
	
	while ((nread = read(fd, szline, 16)) > 0)
	{
		printf("%05u-%08d: ", line++, pos);
		pos += nread;
		int i = 0;
		for  (i = 0; i < nread; i++)
		{
			unsigned char val = *(unsigned char*)(szline + i);
			printf ("%02X ", val);
			if (i == 7) printf (" ");
			
		}
		while (i < 16)
		{
			i++;
			printf ("   ");
		}
		printf("; ");
		
		for (i = 0; i< nread; i++) printf("%c", szline[i]);
		
		printf("\n");
	}
	close (fd);
	return 0;

}
