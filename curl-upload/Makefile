#!/bin/sh
FLAG=-D_D3EBUG_ -D_XOPEN_SOURCE
gk:gk.o iniHelper.o gkutils.o
	gcc -o gk gk.o iniHelper.o gkutils.o  -L /usr/lib64 -lcurl -lcrypt
gk.o:gk.c
	gcc -c gk.c $(FLAG) -I /usr/include/
gkutils.o:gkutils.c
	gcc -c gkutils.c $(FLAG) -I /usr/include/
iniHelper.o:iniHelper.c
	gcc -c iniHelper.c $(FLAG)  -I /usr/include/
clean:
	rm -fr *.o gk 

