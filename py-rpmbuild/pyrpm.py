#!/usr/bin/env python
# -*- coding: utf-8 -*-
import commands
import os, sys, shutil
import ConfigParser

import json
import urllib
import urllib2
import socket
import time
import uuid
import stat
import datetime
#import md5
import hashlib

import smtplib
from email.mime.text import MIMEText
from email.header import Header
import sys
reload(sys)
sys.setdefaultencoding( "utf-8" )

import re
import pdb
import os
import pwd 


user="admin"

def get_flist(dirs):
	ret = []
	for dirpath,dirnames,filenames in os.walk(dirs):
		for fname in filenames:
			fullpath=os.path.join(dirpath,fname)
			if(os.path.isfile(fullpath)):
				ret.append(fullpath)
	return ret
        
if __name__ == '__main__':

        if(len(sys.argv) != 3):
                print "usage: %s tgz specfile" % sys.argv[0]
                sys.exit(1)

        tgz = sys.argv[1]
        spec = sys.argv[2]

        if(os.path.isfile(tgz) == False):
                print "can not find file %s" % tgz
                sys.exit(1)
        if(os.path.isfile(spec) == False):
                print "can not find file %s" % spec
                sys.exit(1)

        dstdir="/home/%s" % user

        if (os.path.isdir(dstdir) == False):
                print "can not find dir %s" % dstdir
                sys.exit(1)
  

	dirs=['BUILD',"BUILDROOT","RPMS","SOURCES","SPECS","SRPMS"]
        maindir="%s/rpmbuild/%s" % (dstdir, os.getpid())

	if(os.path.isdir(maindir)):
        	shutil.rmtree(maindir)
        for ite in dirs:
                pid=os.getpid()
                dpath="%s/rpmbuild/%s/%s" % (dstdir, pid, ite)
                if (os.path.isdir(dpath) == False):
                        os.makedirs(dpath, 0777)
      	ch = "/usr/bin/chown"
	if(os.path.isfile(ch) == False):
		ch = "/bin/chown"
		if(os.path.isfile(ch) == False):
			print "can not find command chown"
			sys.exit(1) 
	cmd="%s -R %s:%s %s" % (ch, user, user, os.path.dirname(maindir))
	status, msg = commands.getstatusoutput(cmd)
	if(status != 0):
		print "change owner of %s failed: [%s][%s]" % (maindir,cmd,msg)
		sys.exit(1)
        ### copy file
        dstfile =  "%s/SOURCES/%s" % (maindir, os.path.basename(tgz))
        #print dstfile
        shutil.copyfile(tgz, dstfile)
        if(os.path.isfile(dstfile) == False):
                print "copy file %s failed" % dstfile
                sys.exit(1)
        cmd="/usr/bin/sudo -u %s /usr/bin/rpmbuild -bb --define '_topdir %s'  %s" % (user, maindir, spec)
	#print cmd
        status, msg = commands.getstatusoutput(cmd)
        if(status != 0):
		print msg
                sys.exit(1)
        ## get rpm files
        rpmdir="%s/RPMS" % maindir
        flist = get_flist(rpmdir)
	size=len(flist)
	ret={}
	ret['data'] = flist
	ret['size'] = size
	print json.dumps(ret)
        ######


