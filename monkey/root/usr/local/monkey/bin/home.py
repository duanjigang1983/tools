#!/usr/bin/env python
#coding utf-8

import os,sys
import re

class Home:
    def __init__(self):
        self.root = os.path.split( os.path.realpath( sys.argv[0] ) )[0]
        self.rootlib = re.sub("bin","lib/",self.root)
        self.rootconf = re.sub("bin","conf/",self.root)
        self.rootplugin = re.sub("bin","plugin/",self.root)
    def setLib(self):
        sys.path.append(self.rootlib)
