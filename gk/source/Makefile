#!/bin/sh
FLAG=-D_D2EBUG_ -D_XOPEN_SOURCE -O2
gk:gk.o iniHelper.o gkutils.o act_uploadpkg.o act_listrepo.o act_listpkg.o
	gcc -o gk gk.o iniHelper.o gkutils.o act_uploadpkg.o act_listrepo.o act_listpkg.o  -L /usr/lib64 -lcurl -lcrypt
gk.o:gk.c
	gcc -c gk.c $(FLAG) -I /usr/include/
gkutils.o:gkutils.c
	gcc -c gkutils.c $(FLAG) -I /usr/include/
act_uploadpkg.o:act_uploadpkg.c
	gcc -c act_uploadpkg.c $(FLAG) -I /usr/include/
act_listrepo.o:act_listrepo.c
	gcc -c act_listrepo.c $(FLAG) -I /usr/include/
act_listpkg.o:act_listpkg.c
	gcc -c act_listpkg.c $(FLAG) -I /usr/include/

iniHelper.o:iniHelper.c
	gcc -c iniHelper.c $(FLAG)  -I /usr/include/
clean:
	rm -fr *.o gk 

