#! /bin/bash
echo -e "-=-=-=-=-=-=-=-=-=-Begin-=-=-=-=-=-=-=-=-=-\n"

FILENAME="HttpServer"
FILEPATH="../bin/HttpServer"
RUNFILE="./../bin/HttpServer"

# 检查HttpServer进程是否正在运行
is_run()
{
	#echo "-=-=-=-=- is_run -=-=-=-=-"
	ps_out=$(ps -ef | grep $FILENAME | grep -v 'grep')
	result=$(echo $ps_out | grep "$FILENAME")
	if [[ "$result" != "" ]]; then
		return 0
	else
		return 1
	fi
}

# 检测文件../bin/HttpServer是否存在
is_exist()
{
	#echo "-=-=-=-= is_exist -=-=-=-=-"
	if [ -x $FILEPATH ]; then
		return 0
	else
		return 1
	fi
}

# 检查HttpServer服务运行状态
state_process()
{
	echo "-=-=-=-=- state -=-=-=-=-"
	if is_run; then
		echo "HttpServer state: Activity"
	else
		echo "HttpServer state: Termination"
	fi
}

# 启动HttpServer程序
start_process()
{
	echo "-=-=-=-=- start -=-=-=-=-"
	if is_run; then
		echo "HttpServer is running ..."
	else
		if is_exist; then
			$($RUNFILE)
			if is_run; then
				echo "Start HttpServer Sucess !!!"
			else
				echo "Start HttpServer Fail !!!"
			fi
		else
			echo "First, please use Makefile to build HttpServer !!!"
		fi
	fi
}

# 停止HttpServer程序
stop_process()
{
	echo "-=-=-=-=- stop -=-=-=-=-"
	PROCESS=$(ps -ef | grep "$FILENAME" | grep -v grep | grep -v PPID | awk '{ print $2}')
	for i in $PROCESS
	do
		kill -9 $i
	done

	if is_run; then
		echo "Stop HttpServer Fail !!!"
	else
		echo "Stop HttpServer Sucess !!!"
	fi
}

# 重启HttpServer程序
restart_process()
{
	echo "-=-=-=-=- restart -=-=-=-=-"
	stop_process
	echo "启动中......"
	sleep 3
	start_process
}

if [ $# -eq 0 ];then
	echo "使用格式："
	echo "./tool Option"
    echo "Option:"
	echo "	state - 查看HttpServer服务运行状态"
	echo "	start - 启动HttpServer服务"
	echo "	restart - 重启HttpServer服务"
	echo "	stop - 停止HttpServer服务"    
	exit 0
fi

case $1 in
	state)
		state_process
		;;
	start)
		start_process
		;;
	restart)
		restart_process
		;;
	stop)
		stop_process
		;;
esac

echo -e "\n-=-=-=-=-=-=-=-=-=-=End=-=-=-=-=-=-=-=-=-=-"
