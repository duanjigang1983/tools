#!/usr/bin/python
# coding=utf-8
## created by duanjigang1983@2014-02-24 to construct an monitor platform

import simplejson as json
import urllib2
import sys
import ConfigParser as cparser
import thread
import time
import commands
import home
root = home.Home()
root.setLib()
import myapi as my
api = my.Myapi(root.rootplugin)



if __name__ == '__main__':
    api.parse_args(sys.argv)
    locks = []
    api.get_conf_list()
    for num in range (0, api.threadnum):
      #print "===%d" % (num)
      lock=thread.allocate_lock()
      lock.acquire()
      locks.append(lock)
      thread.start_new_thread(api.check_secs, (num,lock)) 
    #while api.wait_for_over() != True:
     # time.sleep(0.1)
    for lock in locks:
      while lock.locked():
        time.sleep(0.1)
        pass;
    ### wait for thread to exit, wait_for_over is not reliable
    #time.sleep(1)
     
