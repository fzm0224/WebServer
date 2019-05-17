#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"
#include "cJSON.h"
#include "log.h"

#define MAX_FD 65536
#define MAX_EVENT_NUMBER 10000

extern int addfd( int epollfd, int fd, bool one_shot );
extern int removefd( int epollfd, int fd );

const char * doc_root = NULL;
char message_log[64] = {0};
char warning_log[64] = {0};

void addsig( int sig, void( handler )(int), bool restart = true )
{
	struct sigaction sa;
	memset( &sa, '\0', sizeof( sa ) );
	sa.sa_handler = handler;
	if( restart )
	{
		sa.sa_flags |= SA_RESTART;
	}
	sigfillset( &sa.sa_mask );
	assert( sigaction( sig, &sa, NULL ) != -1 );
}

void show_error( int connfd, const char* info )
{
	msglog( MSG_INFO, "%s\n", info );
	send( connfd, info, strlen( info ), 0 );
	close( connfd );
}

void read_config(char *buf)
{
	FILE *fp = fopen("../conf/HttpServer.conf", "r");
	if (fp == NULL)
	{   
		printf("open HttpServer.conf error!\n");
		return ; 
	}   

	int i = 0;
	char ch; 
	while ((ch = fgetc(fp)) != EOF)
	{   
		buf[i++] = ch; 
	}
}

void analysis_json(char *jsonStr, char *ip, char *port, char *path, char *pthread_num, char *mes_log, char *war_log)
{
	cJSON *root = NULL;
	cJSON *server = NULL;
	cJSON *log = NULL;
	cJSON *item = NULL;
	
	root = cJSON_Parse(jsonStr);
	if (!root)
	{
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
		return ;
	}
	else
	{
		server = cJSON_GetObjectItem(root, "server");
		
		item = cJSON_GetObjectItem(server, "IP");
		strcpy(ip, cJSON_Print(item));
		sscanf(ip, "\"%[^\"]", ip);

		item = cJSON_GetObjectItem(server, "Port");
		strcpy(port, cJSON_Print(item));
		sscanf(port, "\"%[^\"]", port);

		item = cJSON_GetObjectItem(server, "Path");
		strcpy(path, cJSON_Print(item));
		sscanf(path, "\"%[^\"]", path);

		item = cJSON_GetObjectItem(server, "PthreadNum");
		strcpy(pthread_num, cJSON_Print(item));
		sscanf(pthread_num, "\"%[^\"]", pthread_num);

		log = cJSON_GetObjectItem(root, "log");
		
		item = cJSON_GetObjectItem(log, "MessageLog");
		strcpy(mes_log, cJSON_Print(item));
		sscanf(mes_log, "\"%[^\"]", mes_log);

		item = cJSON_GetObjectItem(log, "WarningLog");
		strcpy(war_log, cJSON_Print(item));
		sscanf(war_log, "\"%[^\"]", war_log);
	}
}

int main( int argc, char* argv[] )
{
	int ret = 0;

	char ip[32] = {0};
	char port_str[16] = {0};
	char path[64] = {0};
	char pthread_num[8] = {0};
	char jsonBuf[4096] = {0};

	//读配置文件并解析
	read_config(jsonBuf);
	analysis_json(jsonBuf, ip, port_str, path, pthread_num, message_log, warning_log);

	int port = atoi(port_str);
	int pthreadNum = atoi(pthread_num);
    if (pthreadNum > 16 || pthreadNum < 4)
    {
        pthreadNum = 8;
    }

	doc_root = path;

	//初始化日志系统
	char app_name[32] = "HttpServer";
	ret = msgInit(app_name);
	assert(ret != -1);

	//服务程序后台化，使用Linux的库函数daemon，来实现程序后台化，也可以自己实现。
	ret = daemon(0, 0);
	if (ret == -1)
	{
	  msglog( MSG_WARN, "%s %d: %s\n", __FILE__, __LINE__, "daemon() func error!" );
		return -1;
	}

	//忽略SIGPIPE信号
	addsig( SIGPIPE, SIG_IGN );

	//创建线程池
	threadpool< http_conn >* pool = NULL;
	try
	{
		pool = new threadpool< http_conn >(pthreadNum);
	}
	catch( ... )
	{
		return 1;
	}

	//预先为每个可能的客户连接分配一个http_conn对象
	http_conn* users = new http_conn[ MAX_FD ];
	assert( users );
	int user_count = 0;

	int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
	assert( listenfd >= 0 );

	//SO_LINGER选项用于控制close系统调用在关闭TCP连接时的行为。设置SO_LINGER选项的值时，我们需要使用setsockopt系统调用传递一个linger类型的结构体。
	//这里linger结构体设置为1，0的意义为：close系统调用立即返回，TCP模块将丢弃被关闭的socket对应的TCP发送缓冲区中残留的数据，同时给对方发送一个复位报文段。这种情况给服务器提供了异常终止一个连接的方法。
	struct linger tmp = { 1, 0 };
	setsockopt( listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof( tmp ) );

	struct sockaddr_in address;
	bzero( &address, sizeof( address ) );
	address.sin_family = AF_INET;
	inet_pton( AF_INET, ip, &address.sin_addr );
	address.sin_port = htons( port );

	ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
	assert( ret >= 0 );

	ret = listen( listenfd, 5 );
	assert( ret >= 0 );

	epoll_event events[ MAX_EVENT_NUMBER ];
	int epollfd = epoll_create( 5 );
	assert( epollfd != -1 );
	addfd( epollfd, listenfd, false );
	http_conn::m_epollfd = epollfd;

	while( true )
	{
		int number = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
		if ( ( number < 0 ) && ( errno != EINTR ) )
		{
			msglog( MSG_WARN, "%s %d: %s\n", __FILE__, __LINE__, "epoll_wait() func error!" );
			break;
		}

		for ( int i = 0; i < number; i++ )
		{
			int sockfd = events[i].data.fd;
			if( sockfd == listenfd )
			{
				struct sockaddr_in client_address;
				socklen_t client_addrlength = sizeof( client_address );
				int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
				if ( connfd < 0 )
				{
			        msglog( MSG_WARN, "%s %d: %s\n", __FILE__, __LINE__, "accept() func error!" );
					continue;
				}
				if( http_conn::m_user_count >= MAX_FD )
				{
					show_error( connfd, "-=-=-=-=-=- Internal server busy -=-=-=-=-=-" );
					continue;
				}

				//初始化客户连接
				users[connfd].init( connfd, client_address );
			}
			else if( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) )
			{
				//如果有异常，直接关闭客户连接
				users[sockfd].close_conn();
			}
			else if( events[i].events & EPOLLIN )
			{
				//根据读的结果，决定是将任务添加到线程池，还是关闭连接
				if( users[sockfd].read() )
				{
					pool->append( users + sockfd );
				}
				else
				{
					users[sockfd].close_conn();
				}
			}
			else if( events[i].events & EPOLLOUT )
			{
				//根据写的结果，决定是否关闭连接
				if( !users[sockfd].write() )
				{
					users[sockfd].close_conn();
				}
			}
			else
			{}
		}
	}

	msgLogClose();
	close( epollfd );
	close( listenfd );
	delete [] users;
	delete pool;
	return 0;
}
