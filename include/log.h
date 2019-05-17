#ifndef LOG_H
#define LOG_H

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define LOG_PROCNAME      0x00000001	/* msglog 输出日志时打印程序名 */
#define LOG_PID           0x00000010    /* msglog 输出日志时打印进程 PID */
#define LOG_PERROR        0x00000100    /* msglog 是否把告警内容输出到stderr */
#define NLO_PROCNAME      0x11111110    /* msglog 不输出程序名 */
#define NLO_PID           0x11111101    /* msglog 不输出进程 PID */
#define NLO_PERROR        0x11111011    /* msglog 不输出告警到stderr */

#define MSG_INFO          0x00000001    /* msglog 输出到告警日志文件中 */
#define MSG_WARN          0x00000010    /* msglog 输出到普通日志文件中 */
#define MSG_BOTH          MSG_INFO|MSG_WARN		/* msglog 输出到普通和告警日志文件中 */

//#define LOG_MESSAGE_FILE  "/home/fzm/log/message.log"	/* 系统程序运行日志信息文件 */
#define LOG_MESSAGE_DFMT  "%m-%d %H:%M:%S"				/* 日志信息时间格式字串 */
//#define LOG_POSTFIX_MESS  "%y%m"       					/* 程序运行日志信息文件后缀，带日期后缀 */
#define LOG_POSTFIX_MESS  ""  	     					/* 程序运行日志信息文件后缀，不带日期后缀 */

//#define LOG_WARNING_FILE  "/home/fzm/log/warning.log"   /* 系统程序运行告警日志文件 */
#define LOG_WARNING_DFMT  "%m-%d %H:%M:%S"				/* 告警信息时间格式字串 */
#define LOG_POSTFIX_WARN  ""           					/* 程序运行告警日志文件后缀 */

//写日志函数
int msglog(int mtype, const char *outfmt, ...);
//对日志格式化 
int msgLogFormat(int mopt, const char *mdfmt, int wopt, const char *wdfmt);
//打开日志文件
int msgLogOpen(char *ident, char *mpre, const char *mdate, char *wpre, const char *wdate);
//关闭日志文件
int msgLogClose(void);

/* 设置开始时间 0=ok */
long begusec_process(void);
/* 返回usecond 从 begusec_process历时 */
long getusec_process(void);

/* 日志文件初始化,也可以通过msgLogOpen进行初始化 */
int msgInit(char *pName);

#endif
