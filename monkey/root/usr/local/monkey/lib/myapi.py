#!/usr/bin/python
# coding=utf-8
import simplejson as json
import urllib2
import sys
import ConfigParser as cparser
import thread
import time
import commands
import getopt
import thread
#conflist = { }
#threadnum=3

class Myapi:
    def __init__(self, plugindir):
      self.name="test"
      self.conflist = { }
      self.seclist = {}
      self.secnum = 0
      self.threadnum = 10
      self.plugindir = plugindir
      self.conf=""
    #####################################
    def Usage(self):
      print 'monkey.py usage:'
      print '-h,--help: print help message.'
      print '-v, --version: print script version'
      print '-c, --config: input from a file'
      print '-m --multi: multi-thread number'
#######################################

    def parse_args(self,argv):
      try:
          opts, args = getopt.getopt(argv[1:], 'hvc:m:', ['config=', 'milti='])
      except getopt.GetoptError, err:
        print str(err)
        self.Usage()
        sys.exit(2)
      for o, a in opts:
        if o in ('-h', '--help'):
          self.Usage()
          sys.exit(1)
        elif o in ('-v', '--version'):
          print "monkey version 20140224"
          sys.exit(0)
        elif o in ('-c', '--config'):
          self.conf=a
        elif o in ('-m','--multi'):
          self.threadnum=int(a)
        else:
          print 'unhandled option'
          sys.exit(3)
    ##load rule list of ace monitor
    
    
    def get_conf_list(self):
      config = cparser.ConfigParser()
      ret = config.read(self.conf)
      if len(ret) < 1:
    #failed
        sys.exit(-1)

      for section in config.sections():
        #print "In section [%s]" % section
        self.conflist[section] = {}
        self.seclist[self.secnum] = section
        self.secnum += 1;
        for (key, value) in config.items(section):
          #print "Key '%s' has value '%s'" % (key, value)
          self.conflist[section][key] = value
          self.conflist[section]['over'] = 0
      return 1
	
    
    def check_secs(self, index, lock):
      size=len(self.conflist)
      #print "size:%d,index:%d,num %d\n" % (size, index,self.secnum)
      #for i  in self.conflist:  
       
      for ptr in range (0, self.secnum):
        i=self.seclist[ptr]
        if (ptr % self.threadnum) == index:
          #print "%d %d %s\n" % (ptr, index, i)
          j = json.loads(self.conflist[i]['data'])
          if self.conflist[i]['plugin'] == 'tcp':
            value = self.check_tcp(self.conflist[i])
          if self.conflist[i]['plugin'] == 'udp':
            value = self.check_udp(self.conflist[i])
          if self.conflist[i]['plugin'] == 'disk':
            value = self.check_disk(self.conflist[i])
          if self.conflist[i]['plugin'] == 'task':
            value = self.check_task(self.conflist[i])
          if self.conflist[i]['plugin'] == 'mem':
            value = self.check_mem(self.conflist[i])
          if self.conflist[i]['plugin'] == 'cpu':
            value = self.check_cpu(self.conflist[i])
          if self.conflist[i]['plugin'] == 'load':
            value = self.check_load(self.conflist[i])
          #print "Thread_%d %s %s %s %d" % (index, j['host'],j['port'], self.conflist[i]['plugin'],  value) 
          #print "%d %s %s %s %d" % (time.time(), j['host'],j['port'], self.conflist[i]['plugin'],  value) 
          print "%s %d %s %s %d\n" % (self.conflist[i]['name'], time.time(), j['host'], self.conflist[i]['plugin'],  value)
          sys.stdout.flush()
          #self.conflist[i]['over'] = 1
      #thread.exit_thread()
      for i  in self.conflist:
        self.conflist[i]['over'] = 1
      #thread.exit_thread()
      lock.release();

    def wait_for_over(self):
      for i in self.conflist:
        if self.conflist[i]['over'] != 1:
          return False
      return True

    #############check tcp
    def check_tcp (self, data):
      j = json.loads(data['data'])
      strsend=""
      strrecv=""
      if j.has_key('send'):
        strsend="-s '%s' " % (j['send'])
      if j.has_key('recv'):
        strrecv="-e '%s' " % (j['recv'])
      cmd="%s%s -H %s -p %s -w 2 %s %s" % (self.plugindir, "check_tcp", j['host'],j['port'], strsend, strrecv)
      #print "run:%s (%s)" % (cmd,j['return'])
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1
      #print "%s %s %s %d" % (j['host'],j['port'], data['plugin'],  value)
     #############check tcp
    def check_udp (self, data):
      j = json.loads(data['data'])
      cmd="%s%s -H %s -p %s -s '%s' -e '%s'" % (self.plugindir, "check_udp", j['host'], j['port'], j['send'], j['recv'])
      #print "run:%s" % (cmd)
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1
     #############check disk
    def check_disk (self, data):
      j = json.loads(data['data'])
      cmd="%s%s %s %s %s" % (self.plugindir, "check_sys", j['host'], "check_disk", j['max'])
      #print "run:%s" % (cmd)
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1

     #############check task
    def check_task (self, data):
      j = json.loads(data['data'])
      cmd="%s%s %s %s %s" % (self.plugindir, "check_sys", j['host'], "check_task", j['command'])
      #print "run:%s" % (cmd)
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1

     #############check task
    def check_mem (self, data):
      j = json.loads(data['data'])
      cmd="%s%s %s %s %s" % (self.plugindir, "check_sys", j['host'], "check_mem", j['max'])
      #print "run:%s" % (cmd)
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1
     #############check cpu
    def check_cpu (self, data):
      j = json.loads(data['data'])
      cmd="%s%s %s %s %s" % (self.plugindir, "check_sys", j['host'], "check_cpu", j['max'])
      #print "run:%s" % (cmd)
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1
     #############check load
    def check_load (self, data):
      j = json.loads(data['data'])
      cmd="%s%s %s %s %s" % (self.plugindir, "check_sys", j['host'], "check_load", j['max'])
      #print "===========run:%s" % (cmd)
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1





