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
#conflist = { }
#threadnum=3

class Myapi:
    def __init__(self, plugindir):
      self.name="test"
      self.conflist = { }
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
        for (key, value) in config.items(section):
        #print "Key '%s' has value '%s'" % (key, value)
          self.conflist[section][key] = value
          self.conflist[section]['over'] = 0
      return 1
      data = urllib2.urlopen('http://cloud.tmall.com/jstApi.do?api=vm.list.ip')
      j = json.load(data)
    #print j.keys()
      ip_num=j['total']

      if j['isSuccess'] != True:
    #if j['isSuccess'] != False:
        return 1

      for i in range(0,ip_num):
      #print "%u-%s" % (i,j['data'][i])
        print "%s" % (j['data'][i])
	
    def check_secs(self, index, xxx):
      size=len(self.conflist)
      num=0
      #print "index:%d" % index
      for i  in self.conflist: 
        num=num+1
        if num % self.threadnum == index:
        #print "dict[%s]=%s" % (i,conflist[i])
          
          j = json.loads(self.conflist[i]['data'])
          if self.conflist[i]['plugin'] == 'tcp':
            value = self.check_tcp(self.conflist[i])
          if self.conflist[i]['plugin'] == 'udp':
            value = self.check_udp(self.conflist[i])
          print "Thread_%d %s %s %s %d" % (index, j['host'],j['port'], self.conflist[i]['plugin'],  value) 
          self.conflist[i]['over'] = 1
      thread.exit_thread()

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
      cmd="%s%s -H %s -p %s -t 2 %s %s" % (self.plugindir, "check_tcp", j['host'],j['port'], strsend, strrecv)
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
      cmd="%s%s -H %s -p %s -t 2 -s '%s' -e '%s'" % (self.plugindir, "check_udp", j['host'], j['port'], j['send'], j['recv'])
      #print "run:%s" % (cmd)
      line = commands.getoutput(cmd)
      value = line.find(j['return'])
      if value == -1:
        return 0
      else:
        return 1
     
