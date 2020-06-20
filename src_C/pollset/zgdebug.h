#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef LINUX
#include <sys/syscall.h>
#endif
#ifdef AIXPPC
#include <pthread.h>
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <stdarg.h>
#ifndef _ZGDEBUG_INCLUDE_
#define _ZGDEBUG_INCLUDE_
#define DEBUG_BUFFER_MAX_LENGTH 200
#define ENABLEZGDEBUG
//#define ZPRINTF(...) {if((NULL != getenv("ZDEBUG")) && (NULL != getenv("ZNODEBUG")?!strstr(getenv("ZNODEBUG"),__FILE__):1)) {printf(__VA_ARGS__);}}
/*
ZDEBUG , to check whether we want to print out debug or not.
ZNODEBUG, disable the debug in these files.
ZODEBUG, only display the debug info for these files.
*/
#ifdef ENABLEZGDEBUG
#ifdef LINUX
#define GETTID syscall(SYS_gettid)
#endif
#ifdef AIXPPC
#define GETTID pthread_self()
#endif
#define ZPRINTF2(args...) { if(NULL != getenv("ZDEBUG")){ \
	struct timeval tv; \
	gettimeofday(&tv,NULL); \
	unsigned long long  tstamp = (long long)tv.tv_sec*1000+tv.tv_usec/1000; \
	int is = strlen(__FILE__) - 1; \
	while(is>=0) {if(__FILE__[is] == '/') break;is--;} \
	char * shortname = (char *)((char *)__FILE__+is+1); \
	if((NULL == getenv("ZNODEBUG")) || ((NULL != getenv("ZNODEBUG")) && (strstr(getenv("ZNODEBUG"),shortname) == NULL))) { \
		if((NULL == getenv("ZODEBUG")) || ((NULL != getenv("ZODEBUG")) && (strstr(getenv("ZODEBUG"),shortname) != NULL))) { \
			if((NULL == getenv("ZNODEBUGF")) || ((NULL != getenv("ZNODEBUGF")) && (strstr(getenv("ZNODEBUGF"),__FUNCTION__) == NULL))) { \
				if((NULL == getenv("ZODEBUGF")) || ((NULL != getenv("ZODEBUGF")) && (strstr(getenv("ZODEBUGF"),__FUNCTION__) != NULL))) { \
					int len = (int)strlen(__FILE__)-30; \
					is =((len<0)?0:len); \
					char * psub=(char *)((char *)__FILE__+is); \
					printf("zg.[%ld][%llu]%30.30s:%5d,%s().",GETTID,tstamp,psub,__LINE__,__FUNCTION__); \
					printf(args);\
}/*ZODEBUGF*/}/*ZODEBUG*/}/*ZNODEBUGF*/}/*ZNODEBUG*/}/*ZDEBUG*/}/*ZPRINTF2*/

#define ZPRINTF(args...) { if(NULL != getenv("ZDEBUG")){ \
	struct timeval tv; \
	gettimeofday(&tv,NULL); \
	unsigned long long  tstamp = (long long)tv.tv_sec*1000+tv.tv_usec/1000; \
	int is = strlen(__FILE__) - 1; \
	while(is>=0) {if(__FILE__[is] == '/') break;is--;} \
	char * shortname = (char *)((char *)__FILE__+is+1); \
	if((NULL == getenv("ZNODEBUG")) || ((NULL != getenv("ZNODEBUG")) && (strstr(getenv("ZNODEBUG"),shortname) == NULL))) { \
		if((NULL == getenv("ZODEBUG")) || ((NULL != getenv("ZODEBUG")) && (strstr(getenv("ZODEBUG"),shortname) != NULL))) { \
			int len = (int)strlen(__FILE__)-30; \
			is =((len<0)?0:len); \
			char * psub=(char *)((char *)__FILE__+is); \
			printf("zg.[%ld][%llu]%30.30s:%5d,%s().",GETTID,tstamp,psub,__LINE__,__FUNCTION__); \
			printf(args);\
}/*ZODEBUG*/}/*ZNODEBUG*/}/*ZDEBUG*/}/*ZPRINTF2*/

#else
#define ZPRINTF2(args...)
#define ZPRINTF(args...)
#endif
/*
//printf(__VA_ARGS__);\
// void zPrintf( const char* format, ...)
// {
// 	char buffer[DEBUG_BUFFER_MAX_LENGTH + 1]={0};
// 	struct timeval tv;
// 	va_list arg;
// 	gettimeofday(&tv,NULL);
// 	unsigned long long  tstamp = (long long)tv.tv_sec*1000+tv.tv_usec/1000;
// 	int is = strlen(__FILE__) - 1;
// 	while(is>=0) {if(__FILE__[is] == '/') break;is--;}
// 	char * shortname = (char *)((char *)__FILE__+is+1);
// 	if((NULL == getenv("ZNODEBUG")) || ((NULL != getenv("ZNODEBUG")) && (strstr(getenv("ZNODEBUG"),shortname) == NULL))) {
// 		if((NULL == getenv("ZODEBUG")) || ((NULL != getenv("ZODEBUG")) && (strstr(getenv("ZODEBUG"),shortname) != NULL))) 
// 		{
// 			int len = (int)strlen(__FILE__)-30;
// 			is =((len<0)?0:len);
// 			char * psub=(char *)((char *)__FILE__+is);
// 			va_start (arg, format);
// 			vsnprintf(buffer, DEBUG_BUFFER_MAX_LENGTH, format, arg);
// 			va_end (arg);
// 			printf( "zg.[%ld][%llu]%30.30s:%5d,%s().%s",syscall(SYS_gettid),tstamp,psub,__LINE__,__FUNCTION__, buffer );
// 		}
// 	}
// }
*/
#endif