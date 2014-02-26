Monkey is a tool developed in Python, which is used to monitor system service (such as tcp port, udp port, tasks,mem).
 and applications, to add something to monitor, what you need to do, is just adding plugins in dir  'plugin' and
inserting your monitor hosts and configurations in the conf file.

To run monkey:

$ python monkey.py -c configfile -m 10

-c: specify the configfile
you may get a config file by modifing conf/rule.conf
-m: how many threads are used to finish the monitor task

# for example:

$./bin/monkey.py -c conf/rule.conf  -m 20

Then we got output like below:
#################################################
Thread_1 127.0.0.1 22 tcp 1
Thread_4 www.baidu.com 80 tcp 1
Thread_5 www.126.com 80 tcp 1
Thread_3 8.8.8.8 53 tcp 1
Thread_2 8.8.8.8 53 udp 0
############################################
The last row show you the result  of each monitor task, '1' means ok,while '0'
mean error or failure.


more about Monkey:
The monitoring function of developed with muilti-thread based on nagios plugins. self-developed
plugins are also supported by Monkey.

Key point:
	You are suggested to read and modify the 'data' items of each monitor
object in the config file

------------ by duanjigang1983 <duanjigang1983@gmail.com> to be updated
