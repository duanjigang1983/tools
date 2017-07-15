#!/bin/sh
FLAG=-D_DEBUG_  -O2 -I /usr/include/rpm
#LIBS=-L /usr/lib64  -lrpmio -lrpmdb -lrpm
LIBS=-L /usr/lib64  -lrpmio  -lrpm
crp:crp.o 
	gcc -o crp $(LIBS)  crp.o 
crp.o:crp.c
	gcc -c crp.c $(FLAG) 
clean:
	rm -fr *.o crp 

