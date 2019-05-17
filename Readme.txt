本程序的使用：
	1. 打开./conf目录下的HttpServer.conf配置文件，修改一下参数：
		IP：服务对应的IP。
		Port：服务监听的端口。
		Path：服务提供请求文件存储的路径。
		PthreadNum：线程池中线程的数量，建议取值在4-16之间，如不再此范围，默认取值为8。
		MessageLog：普通日志文件存储的路径及名称。注意：这里一定要确保该文件路径是存在的，且有访问权限。
		WarningLog：告警日志文件存储的路径及名称。注意：这里一定要确保该文件路径是存在的，且有访问权限。
	然后保存退出。
	2. 使用Makefile进行编译。
	3. 使用./tools中的工具tool.sh脚本管理HttpServer程序。



关于tool.sh工具的使用：
    命令格式：./tool.sh argv
    参数argv的取值：
    state：查看HttpServer服务的运行状态。
    start：启动HttpServer服务程序。
    stop：停止HttpServer服务进程。
    restart：重启HttpServer服务程序。
    注：argv只能取下面参数中的一个。



关于使用Json作为配置文件：
	其实Json作为配置文件不是一个好的选择。因为Json格式的语法要求非常严格，并且其做配置文件最大的缺陷是缺乏注释，Json文件中无法使用注释。
	但是由于目前水平有限，想到的作为配置文件的格式化语言只有Jscon，并且C/C++拥有Json的小型解释器程序，可以直接使用。
