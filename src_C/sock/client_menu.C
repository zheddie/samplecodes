#include <stdio.h>
#include <string.h>	// memset
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#define SOCK_TEST
#include "Xj9SharedHeader.H"


#define AGENT_VERSION 7ull


typedef struct
{
  void * pointer;
} format2;

static int debug = 0;
static int automode = 0;
static uint64 ran_ktid = 0;

char * j9time(uint64 time)
{
  time_t timestamp = (time_t)(time/1000000);
  if (timestamp == 0)
    return "                        ";
  
  char * str = ctime(&timestamp);
  str[24] = '\0';
  return str;
}

int random(int low, int high)
{
  int r = rand();
  int ran = r % (high - low + 1) + low;
  return ran;
}

void dumpChar(char * start, int len)
{
  printf(" *");
  for (int j=0;j<0x10;j++)
  {
    if (j<len)
    {
      if (isprint(start[j]))
	printf("%1c", start[j]);
      else
	printf(" ");
    }
    else
      printf("*");
  }
  printf("*");
}

void dumpHex(void * start, int len)
{
  char * p = (char *)start;

  for (int i=0;i<len;i++)
  {
    if ((i % 0x10) == 0) printf("%04X  ", i);	/* print offset every 0x10 */
    if ((i % 0x4)  == 0) printf(" ");		/* add space every 0x4 */
    printf("%02X", (int)p[i]);
    if (((i+1) % 0x10) == 0)
    {
      dumpChar(p+i-0xf, 0x10);
      printf("\n");
    }
  }
  int k = len % 0x10;
  for (int j=k;j<0x10;j++)
  {
    if ((j % 0x4)  == 0) printf(" ");		/* add space every 0x4 */
    printf("  ");
  }
  dumpChar(p+len-k, k);
  printf("\n");
}

void dumpSeg(char * cbuffer, uint32 offset)
{
  //printf("offset = %x\n", offset);
  Jvm_Segment_List_t * segList = (Jvm_Segment_List_t *)(cbuffer + offset);

  for (int i=0;i<segList->segListCount;i++)
  {
    printf("%016llX %016llX %016llX %016llX %016llX %016llX \n",
	   segList->list[i].segmentId,
	   segList->list[i].start,
	   segList->list[i].alloc,
	   segList->list[i].end,
	   segList->list[i].type,
	   segList->list[i].size);
  }
	   
}

void dump_monitor(Jvm_javaLockMonitor_t * entry, char * name)
{
  printf("  ");
  if (entry->held) printf("held = %d ", entry->held);
  if (entry->enter_count) printf("enter = %d ", entry->enter_count);
  if (entry->slow_count) printf("slow = %d ", entry->slow_count);
  if (entry->recursive_count) printf("recursive = %d ", entry->recursive_count);
  if (entry->spin2_count) printf("spin2 = %d ", entry->spin2_count);
  if (entry->yield_count) printf("yield = %d ", entry->yield_count);
  if (entry->hold_time) printf("hold_time = %lld", entry->hold_time);
  if (entry->object) printf("object = %llx", entry->object);
  printf(" %s\n", name);

}


void recv_command(int sockfd, Jvm_Command_t & command, Jvm_ErrorCode_t & errorCode, void * & buffer, size_t & len)
{
  uint64 version;
  ssize_t recv_len = recv(sockfd, &version, sizeof(version), MSG_WAITALL);
  //printf("version recv_len = %lld\n", recv_len);
  assert(recv_len == sizeof(version));

  if (version > AGENT_VERSION)
  {
    printf("i5/OS Agent at version %lld\n", version);
    printf("client_menu at version %lld\n", AGENT_VERSION);
    printf("You need to upgrade client_menu.\n");
    exit(-1);
  }
  assert(version == AGENT_VERSION);

  recv_len = recv(sockfd, &command, sizeof(command), MSG_WAITALL);	// receive command
  assert(recv_len == sizeof(command));
  //printf("send command %lld\n", command);

  recv_len = recv(sockfd, &errorCode, sizeof(errorCode), MSG_WAITALL);	// receive errorCode
  assert(recv_len == sizeof(errorCode));


  uint64 in_len;
  recv_len = recv(sockfd, &in_len, sizeof(in_len), MSG_WAITALL);	// receive len
  assert(recv_len == sizeof(in_len));
  len = in_len;
  //printf("len = %lld\n", len);

  //printf("recv buffer\n");
  //buffer = malloc(4096);
  //assert(buffer != NULL);
  
  
  if (len > 4096)
  {
    buffer = malloc(len);
    assert(buffer != NULL);
    recv_len = recv(sockfd, buffer, len, MSG_WAITALL);		// recieve buffer
    //printf("recv_len = %lld len = %lld\n", recv_len, len);
    assert(recv_len == len);
  }
  else
  {
    buffer = malloc(4096);
    assert(buffer != NULL);
    recv_len = recv(sockfd, buffer, 4096, MSG_WAITALL);		// recieve buffer
    assert(recv_len == 4096);
  }
}

void send_command(int sockfd, Jvm_Command_t command, void * buffer, size_t len)
{
  uint64 version = AGENT_VERSION;
  ssize_t send_len = send(sockfd, &version, sizeof(version), 0);		// send version
  assert(send_len == sizeof(version));
  
  //printf("send command %lld\n", command);
  send_len = send(sockfd, &command, sizeof(command), 0);		// send command
  assert(send_len == sizeof(command));
  //printf("sending buffer len = %d\n", len);
  //printf("buffer = %p\n", buffer);

  if (len > 4096)
  {
    send_len = send(sockfd, buffer, len, 0);			// send buffer
    //printf("send_len = %d len = %d\n", send_len, len);
    assert(send_len == len);
  }
  else
  {
    send_len = send(sockfd, buffer, 4096, 0);			// send buffer
    assert(send_len == 4096);
  }
  free(buffer);
}

int setup(char * name, int port)
{
  int sockfd;
  struct sockaddr_in serv_addr;
  struct hostent  * host;
  struct in_addr * addr = NULL;
  char * * list;

  host = gethostbyname(name);
  if (!host)
  {
    printf("host ip not found\n");
    exit(-1);
  }
  
  if (host->h_addrtype == AF_INET)
  {
    /* stop if list entry is null */
    for (list=host->h_addr_list; *list!=NULL; list++)
    {
      addr = (struct in_addr *)*list;
    }
  }
  if (!addr)
  {
    printf("No IP found\n");
  }
  /*printf("ip = %s\n", inet_ntoa(*addr));*/
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family		= AF_INET;
  serv_addr.sin_addr		= *addr;
  serv_addr.sin_port		= htons(port);

  //printf("socket\n");
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  assert(sockfd != -1);

  //printf("connect\n");
  int rc = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (rc == -1)
  {
    printf("Could not connect to %s:%d\n",name,port);
    exit(-1);
  }
  //printf("connected\n");
  return sockfd;
}

void server(int sockfd, Jvm_Command_t & command, Jvm_ErrorCode_t & errorCode, void * & buffer, size_t & len, int print)
{
  if (print)
  {
    printf("send buffer\n");
    dumpHex(buffer, 16);
  }
  
  send_command(sockfd, command, buffer, len);
  recv_command(sockfd, command, errorCode, buffer, len);

  if (print)
  {
    printf("received buffer len = 0x%llX\n", len);
    dumpHex(buffer, len);
  }
}

static char hextable[] = "0123456789abcdef";
char cbuf[1024];

char * u2c(Jvm_unicode_t * u_object)
{
  if (!u_object)
    return "null";
  
  int j = 0;
  for (int i=0;i<u_object->length;i++)
  {
    uint16 u = u_object->unicode[i];
    if ((u == 0) || (u > 0x007F))
    {
      cbuf[j++] = '\\';
      cbuf[j++] = 'u';
      cbuf[j++] = hextable[(u&0xf000)>>12];
      cbuf[j++] = hextable[(u&0x0f00)>>8];
      cbuf[j++] = hextable[(u&0x00f0)>>4];
      cbuf[j++] = hextable[(u&0x000f)>>0];
    }
    else      
      cbuf[j++] = u;
  }
  cbuf[j] = '\0';
  return cbuf;
}


void print_data(int sockfd, Jvm_Command_t command, Jvm_ErrorCode_t errorCode, void * buffer, size_t & len)
{
  char * cbuffer = (char *)buffer;
  printf("errorCode = %lld %s\n", errorCode, JvmCodesToStrings::getErrorString(errorCode));

  if (errorCode == Jvm_Success)
  {
    // handle returned data
    switch(command)
    {
	case Jvm_Init:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_init_t * t = (Jvm_init_t *)u.data();
	      printf("thread_count = %d\n", t->thread_count);
	      int64 * ktid = (int64 *)u.getArray(t->ktid);
	      uint8 * type = (uint8 *)u.getArray(t->type);
	      for (int i=0;i<t->thread_count;i++)
	      {
		printf("tid = %06llx type = %d\n", ktid[i], (int)type[i]);
	      }
	    }
	    break;
	case Jvm_Alive:
	    printf("The Agent is alive\n");
	    break;
	case Jvm_hello:	// string
	    {
	      JvmUnpacker u(buffer);
	      Jvm_hello_t * t = (Jvm_hello_t *)u.data();
	      printf("%s\n", u2c(u.getUnicodeObject(t->string)));
	    }
	    break;
/*	case Jvm_TestPrint:
	    {
	      while (*((char*)buffer) != '\0')
	      {
		printf("%s",buffer);
		server(sockfd, command, errorCode, buffer, len, 0);
	      }
	      printf("TestPrint complete\n");
	    }
	    break;*/
	case Jvm_ILEenv:
	case Jvm_env: // environment var
	    {
	      JvmUnpacker u(buffer);
	      Jvm_Env_t * t = (Jvm_Env_t *)u.data();
	      printf("count	= %d\n", t->count);
	      for (int i=0;i<t->count;i++)
	      {
		printf("%s\n", u2c(u.getUnicodeObject(t->strings, i)));
	      }
	    }
	    break;
	case Jvm_getSystemProperties: 
	    {
	      JvmUnpacker u(buffer);
	      Jvm_Properties_t * t = (Jvm_Properties_t *)u.data();
	      printf("count	= %d\n", t->count);
	      for (int i=0;i<t->count;i++)
	      {
		printf("%s\n", u2c(u.getUnicodeObject(t->strings, i)));
	      }
	    }
	    break;
	case Jvm_GetCurrentProperties: 
	    {
	      JvmUnpacker u(buffer);
	      Jvm_Properties_t * t = (Jvm_Properties_t *)u.data();
	      printf("count	= %d\n", t->count);
	      for (int i=0;i<t->count;i++)
	      {
		printf("%s\n", u2c(u.getUnicodeObject(t->strings, i)));
	      }
	    }
	    break;
	case Jvm_ext:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_Ext_t * t = (Jvm_Ext_t *)u.data();
	      printf("extension count = %d\n", t->extension_count);
	      Jvm_ExtFunction_t * extfun = (Jvm_ExtFunction_t *)u.getArray(t->extensions);
	      for (int i=0;i<t->extension_count;i++)
	      {
		printf("func		= %016llX\n", extfun[i].func);
		printf("id		= %s\n", u2c(u.getUnicodeObject(extfun[i].id)));
		printf("desc		= %s\n", u2c(u.getUnicodeObject(extfun[i].desc)));
		printf("param_count	= %d\n", extfun[i].param_count);
		Jvm_Params_t * params = (Jvm_Params_t *)u.getArray(extfun[i].params);
		for (int j=0;j<extfun[i].param_count;j++)
		{
		  printf("j=%d	name = %s\n", j, u2c(u.getUnicodeObject(params[j].name)));
		  printf("		kind = %d\n", params[j].kind);
		  printf("		type = %d\n", params[j].base_type);
		  printf("		null = %d\n", params[j].null_ok);

		}
	      }
	    }
	    break;
	case Jvm_threads:
	    {
	      //printf("Jvm_threads print\n");
	      void * copy = new char[len];
	      memcpy(copy, buffer, len);
	      JvmUnpacker u(copy);
	      Jvm_threads_t * t = (Jvm_threads_t *)u.data();
	      int64 * ktid = (int64 *)u.getArray(t->ktid);
	      printf("thread_count = %d\n", t->thread_count);
	      ran_ktid = random
	      for (int i=0;i<t->thread_count;i++)
	      {
		printf("tid = %06llx ", ktid[i]);
		memcpy(buffer, &ktid[i], sizeof(int64));
		command = Jvm_getThreadInfo;
		server(sockfd, command, errorCode, buffer, len, 0);
		if (errorCode == Jvm_Success)
		{
		  JvmUnpacker u(buffer);
		  Jvm_threadInfo_t * t = (Jvm_threadInfo_t *)u.data();
		  printf("%s ", t->is_daemon ? "daemon" : "normal");
		  printf("%3d ", (int)t->type);

		  printf("%08x %17s ", t->state, JvmCodesToStrings::getThreadStateString(t->state));
		  printf("pri = %d name = \"%s\" ", t->priority, u2c(u.getUnicodeObject(t->threadName))); 
		  printf("group = \"%s\"\n", u2c(u.getUnicodeObject(t->threadGroupName)));
		}
		else
		  printf("ThreadInfo data not available\n");
	      }
	    }
	    break;
	case Jvm_kill:
	    {
	    }
	    break;
	case Jvm_suspendThread:
	    {
	    }
	    break;
	case Jvm_resumeThread:
	    {
	    }
	    break;
	case Jvm_setDump:
	    {
	    }
	    break;
	case Jvm_raiseSignal:
	    {
	    }
	    break;
	case Jvm_stack:
	    {
	      void * copy = new char[len];
	      memcpy(copy, buffer, len);
	      JvmUnpacker u(copy);
	      Jvm_stacksnap_t * t = (Jvm_stacksnap_t *)u.data();
	      printf("frame count = %d\n", t->frame_count);
	      Jvm_framesnap_t * f = (Jvm_framesnap_t *)u.getArray(t->frames);
	      for (int i=0;i<t->frame_count;i++)
	      {
		memcpy(buffer, &f[i].mid, sizeof(f[i].mid));
		command = Jvm_ResolveMethod;
		server(sockfd, command, errorCode, buffer, len, 0);
		JvmUnpacker u1(buffer);
		Jvm_methodInfo_t * minfo = (Jvm_methodInfo_t *)u1.data();
		printf("%5d %llx ", t->frame_count-i, f[i].mid);
		if (f[i].isMethodNative)
		  printf("native 0x%llx ", f[i].aproxNativeStackAddress);
		else if (f[i].jitted)
		  printf("JITCompiled       ");
		else
		  printf("Interpreter       ");
		printf("%s.", u2c(u1.getUnicodeObject(minfo->className)));
		printf("%s",  u2c(u1.getUnicodeObject(minfo->methodName)));
		printf("%s+0x%llx", u2c(u1.getUnicodeObject(minfo->methodSig)), f[i].loc);
		printf("(%s", u2c(u1.getUnicodeObject(minfo->source_file)));
		Jvm_LineNumberEntry_t * lne = (Jvm_LineNumberEntry_t *)u1.getArray(minfo->linenumber_table);
		int32 linenumber = -1;
		for (int j=0;j<minfo->linenumber_count;j++)
		{
		  if (f[i].loc >= lne[j].start_location)
		    linenumber = lne[j].line_number;
		  else
		    break;
		}
		if (linenumber != -1)
		  printf(":%d)\n", linenumber);
		else
		  printf(")\n");
	      }
	    }
	    break;
	/*case Jvm_stack:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_stacksnap_t * t = (Jvm_stacksnap_t *)u.data();
	      printf("frame count = %d\n", t->frame_count);
	      Jvm_framesnap_t * f = (Jvm_framesnap_t *)u.getArray(t->frames);
	      for (int i=0;i<t->frame_count;i++)
	      {
		printf("%llx ", f[i].mid);
		if (f[i].isMethodNative)
		  printf("native 0x%llx ", f[i].aproxNativeStackAddress);
		else if (f[i].jitted)
		  printf("JITCompiled       ");
		else
		  printf("Interpreter       ");
		printf("%016llx\n", f[i].loc);
	      }
	    }
	    break;*/
	case Jvm_suspendAndSnapshot:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_stacksnap_t * t = (Jvm_stacksnap_t *)u.data();
	      printf("frame count = %d\n", t->frame_count);
	      Jvm_framesnap_t * f = (Jvm_framesnap_t *)u.getArray(t->frames);
	      for (int i=0;i<t->frame_count;i++)
	      {
		printf("%llx ", f[i].mid);
		if (f[i].isMethodNative)
		  printf("native 0x%llx ", f[i].aproxNativeStackAddress);
		else if (f[i].jitted)
		  printf("JITCompiled       ");
		else
		  printf("Interpreter       ");
		printf("%016llx\n", f[i].loc);
	      }
	    }
	    break;
	case Jvm_ResolveMethod:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_methodInfo_t * t = (Jvm_methodInfo_t *)u.data();
	      if (t->isMethodNative)
		printf("native ");
	      else
		printf("java   ");
	      printf("%s.", u2c(u.getUnicodeObject(t->className)));
	      printf("%s", u2c(u.getUnicodeObject(t->methodName)));
	      printf("%s\n", u2c(u.getUnicodeObject(t->methodSig)));
	      printf("(%s)\n", u2c(u.getUnicodeObject(t->source_file)));
	      printf("count = %d\n", t->linenumber_count);
	      Jvm_LineNumberEntry_t * lne = (Jvm_LineNumberEntry_t *)u.getArray(t->linenumber_table);
	      for (int i=0;i<t->linenumber_count;i++)
	      {
		printf("0x%016llx linenumber = %d\n", lne[i].start_location, lne[i].line_number);
	      }
	    }
	    break;
	/*case Jvm_getLineNumberTable:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_LineNumberTable_t * t = (Jvm_LineNumberTable_t *)u.data();
	      printf("count = %d\n", t->count);
	      Jvm_LineNumberEntry_t * lne = (Jvm_LineNumberEntry_t *)u.getArray(t->line_table);
	      for (int i=0;i<t->count;i++)
	      {
		printf("%16llx linenumber = %d\n", lne[i].start_location, lne[i].line_number);
	      }
	    }
	    break;*/
	case Jvm_getThreadInfo:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_threadInfo_t * t = (Jvm_threadInfo_t *)u.data();
	      if (t->is_daemon)
		printf("daemon ");
	      else
		printf("normal ");
	      printf("%08x %17s ", t->state, JvmCodesToStrings::getThreadStateString(t->state));
	      printf("pri = %d name = \"%s\" ", t->priority, u2c(u.getUnicodeObject(t->threadName)));
	      printf("group = \"%s\"\n", u2c(u.getUnicodeObject(t->threadGroupName))); 
	    }
	    break;
	case Jvm_ClassLoadingMXBean:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_ClassLoadingMXBean_t * t = (Jvm_ClassLoadingMXBean_t *)u.data();
	      printf("LoadedClassCount			= %d\n", t->loadedClassCount);
	      printf("TotalLoadedClassCount			= %lld\n", t->totalLoadedClassCount);
	      printf("UnloadedClassCount			= %lld\n", t->unloadedClassCount);
	      printf("Verbose					= %s\n", (t->isVerbose ? "true" : "false"));
	    }
	    break;
	case Jvm_CompilationMXBean:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_CompilationMXBean_t * t = (Jvm_CompilationMXBean_t *)u.data();
	      printf("CompilationTimeMonitoringSupported        = %s\n", (t->isCompilationTimeMonitoringSupported ? "true" : "false"));
	      printf("TotalCompilationTime                      = %lld ms\n", t->totalCompilationTime);
	      printf("totalMethodsTranslated                    = %lld\n", t->totalMethodsTranslated);
	      printf("totalMethodsNotTranslated                 = %lld\n", t->totalMethodsNotTranslated);
	    }
	    break;
	case Jvm_ThreadMXBean:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_ThreadMXBean_t * t = (Jvm_ThreadMXBean_t *)u.data();
	      printf("DaemonThreadCount			= %d\n", t->daemonThreadCount);
	      printf("PeakThreadCount				= %d\n", t->peakThreadCount);
	      printf("ThreadContentionMonitoringEnabled	= %s\n", (t->isThreadContentionMonitoringEnabled ? "true" : "false"));
	      printf("ThreadContentionMonitoringSupported	= %s\n", (t->isThreadContentionMonitoringSupported ? "true" : "false"));
	      printf("ThreadCount				= %d\n", t->threadCount);
	      printf("ThreadCpuTimeEnabled			= %s\n", (t->isThreadCpuTimeEnabled ? "true" : "false"));
	      printf("ThreadCpuTimeSupported			= %s\n", (t->isThreadCpuTimeSupported ? "true" : "false"));
	      printf("totalStartedThreadCount			= %lld\n", t->totalStartedThreadCount);

	    }
	    break;
	case Jvm_GarbageCollectorInfo:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_GarbageCollectorInfo_t * t = (Jvm_GarbageCollectorInfo_t *)u.data();
	      printf("Verbose                   = %s\n", (t->isVerbose ? "true" : "false"));
	      printf("pendingFinalizationCount  = %d\n", t->pendingFinalizationCount);
	      printf("collectionCount           = %lld\n", t->collectionCount);
	      printf("collectionTime            = %lld ms\n", t->collectionTime);	
	      printf("GC stage                  = %s\n", JvmCodesToStrings::getGCStageString(t->GC_Stage));
	      Jvm_GC_cycle_t * entry = (Jvm_GC_cycle_t *)u.getStruct(t->last_GC);
	      if (entry->totalId)
	      {
		printf("%5d", entry->totalId);
		printf(" %s %s %s\n", JvmCodesToStrings::getReasonString(entry->reason), JvmCodesToStrings::getMemoryType(entry->subSpaceType), JvmCodesToStrings::getCompactReason(entry->compactReason));
		printf("      GC      %s", j9time(entry->start_time));
		printf(" %s", j9time(entry->end_time));
		printf(" %7lld us\n", entry->end_time-entry->start_time);
		printf("      Mark    %s", j9time(entry->mark_start_time));
		printf(" %s", j9time(entry->mark_end_time));
		printf(" %7lld us\n", entry->mark_end_time-entry->mark_start_time);
		printf("      Sweep   %s", j9time(entry->sweep_start_time));
		printf(" %s", j9time(entry->sweep_end_time));
		printf(" %7lld us\n", entry->sweep_end_time-entry->sweep_start_time);
		if (entry->compactReason != Jvm_CompactNone)
		{
		  printf("      Compact %s", j9time(entry->compact_start_time));
		  printf(" %s", j9time(entry->compact_end_time));
		  printf(" %7lld us\n", entry->compact_end_time-entry->compact_start_time);
		}
		printf("      Tenure  start %10d/%-10d Nursery start %10d/%-10d\n", entry->startTenureFreeKB, entry->startTenureTotalKB, entry->startNurseryFreeKB, entry->startNurseryTotalKB);
		printf("              end   %10d/%-10d         end   %10d/%-10d\n", entry->endTenureFreeKB, entry->endTenureTotalKB, entry->endNurseryFreeKB, entry->endNurseryTotalKB);
		printf("      LOA     start %10d/%-10d Compact moved %10d\n", entry->startTenureLOAFreeKB, entry->startTenureLOATotalKB, entry->movedKB); 
		printf("              end   %10d/%-10d Objects       %10d\n", entry->endTenureLOAFreeKB, entry->endTenureLOATotalKB, entry->movedObjects);
		printf("      Final         %10d/%-10d Weak          %10d/%-10d\n", entry->startFinalizerCount, entry->endFinalizerCount,entry->startWeakReferenceCount, entry->endWeakReferenceCount);
		printf("      Phantom       %10d/%-10d Soft          %10d/%-10d\n", entry->startPhantomReferenceCount, entry->endPhantomReferenceCount, entry->startSoftReferenceCount, entry->endSoftReferenceCount);
		printf("      int_cycle = %d classUnloadCount = %d excessivGCRaised = %d\n", entry->internalCycleCount, entry->classUnloadCount, entry->excessiveGCRaisedCount);
	      }
	      else
		printf("No last GC cycle\n");
	    }
	    break;
	/*case Jvm_GarbageCollectorMXBean:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_GarbageCollectorMXBean_t * t = (Jvm_GarbageCollectorMXBean_t *)u.data();
	      printf("collectionCount           = %lld\n", t->collectionCount);
	      printf("collectionTime            = %lld ms\n", t->collectionTime);	
	      //printf("agentCollectionCount      = %lld\n", t->agentCollectionCount);
	      printf("agentLastCollectionTime   = %lld us\n", t->agentLastCollectionTime);
	    }
	    break;*/
	/*case Jvm_MemoryMXBean:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_MemoryMXBean_t * t = (Jvm_MemoryMXBean_t *)u.data();
	      printf("Verbose                   = %s\n", (t->isVerbose ? "true" : "false"));
	      printf("pendingFinalizationCount  = %d\n", t->pendingFinalizationCount);
	    }
	    break;*/
	case Jvm_getMemoryInfo:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_Memory_Info_t * t = (Jvm_Memory_Info_t *)u.data();
	      printf("sizeOfVoidPtr     = %lld\n", t->sizeOfVoidPtr);
	      printf("gcMode            = %s\n", u2c(u.getUnicodeObject(t->offsetGcMode)));
	      printf("gcInit            = %08llx %10lld\n", t->gcInit,t->gcInit);
	      printf("gcMax             = %08llx %10lld\n", t->gcMax,t->gcMax);
	      printf("gcSoftMax         = %08llx %10lld\n", t->gcSoftMax,t->gcSoftMax);
	      printf("free              = %08llx %10lld\n", t->heapSpaceFree, t->heapSpaceFree);
	      printf("alloc             = %08llx %10lld\n", t->heapSpaceAllocated, t->heapSpaceAllocated);
	      printf("init_sbreak       = %16llx\n", t->init_sbreak);
	      printf("sbreak            = %16llx\n", t->sbreak);
	      printf("cacheStartAddress = %16llx\n", t->cacheStartAddress);
	      printf("cacheSizeBytes    = %08llx %10lld\n", t->sharedClassSize, t->sharedClassSize);
	      printf("Internal Memory\n");
	      dumpSeg(cbuffer, t->offsetInternalMemory);
	      printf("Object Memory\n");
	      dumpSeg(cbuffer, t->offsetObjectMemory);
	      printf("Class Memory\n");
	      dumpSeg(cbuffer, t->offsetClassMemory);
	      printf("Jit Code Memory\n");
	      dumpSeg(cbuffer, t->offsetJitCodeCache);
	      printf("Jit Data Memory\n");
	      dumpSeg(cbuffer, t->offsetJitDataCache);
	    }
	    break;
	case Jvm_GetGCCycleTable:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_GC_cycle_array_t * t = (Jvm_GC_cycle_array_t *)u.data();
	      printf("count = %d\n", t->count);
	      Jvm_GC_cycle_t * cycle_table = (Jvm_GC_cycle_t *)u.getArray(t->cycle_table);
	      for (int i=0;i<t->count;i++)
	      {
		Jvm_GC_cycle_t * entry = cycle_table + i;
		if (entry->totalId)
		{
		  printf("%5d", entry->totalId);
		  printf(" %s %s %s\n", JvmCodesToStrings::getReasonString(entry->reason), JvmCodesToStrings::getMemoryType(entry->subSpaceType), JvmCodesToStrings::getCompactReason(entry->compactReason));
		  printf("      GC      %s", j9time(entry->start_time));
		  printf(" %s", j9time(entry->end_time));
		  printf(" %7lld us\n", entry->end_time-entry->start_time);
		  printf("      Mark    %s", j9time(entry->mark_start_time));
		  printf(" %s", j9time(entry->mark_end_time));
		  printf(" %7lld us\n", entry->mark_end_time-entry->mark_start_time);
		  printf("      Sweep   %s", j9time(entry->sweep_start_time));
		  printf(" %s", j9time(entry->sweep_end_time));
		  printf(" %7lld us\n", entry->sweep_end_time-entry->sweep_start_time);
		  if (entry->compactReason != Jvm_CompactNone)
		  {
		    printf("      Compact %s", j9time(entry->compact_start_time));
		    printf(" %s", j9time(entry->compact_end_time));
		    printf(" %7lld us\n", entry->compact_end_time-entry->compact_start_time);
		  }
		  printf("      Tenure  start %10d/%-10d Nursery start %10d/%-10d\n", entry->startTenureFreeKB, entry->startTenureTotalKB, entry->startNurseryFreeKB, entry->startNurseryTotalKB);
		  printf("              end   %10d/%-10d         end   %10d/%-10d\n", entry->endTenureFreeKB, entry->endTenureTotalKB, entry->endNurseryFreeKB, entry->endNurseryTotalKB);
		  printf("      LOA     start %10d/%-10d Compact moved %10d\n", entry->startTenureLOAFreeKB, entry->startTenureLOATotalKB, entry->movedKB); 
		  printf("              end   %10d/%-10d Objects       %10d\n", entry->endTenureLOAFreeKB, entry->endTenureLOATotalKB, entry->movedObjects);
		  printf("      Final         %10d/%-10d Weak          %10d/%-10d\n", entry->startFinalizerCount, entry->endFinalizerCount,entry->startWeakReferenceCount, entry->endWeakReferenceCount);
		  printf("      Phantom       %10d/%-10d Soft          %10d/%-10d\n", entry->startPhantomReferenceCount, entry->endPhantomReferenceCount, entry->startSoftReferenceCount, entry->endSoftReferenceCount);
		  printf("      int_cycle = %d classUnloadCount = %d excessivGCRaised = %d\n", entry->internalCycleCount, entry->classUnloadCount, entry->excessiveGCRaisedCount);
		}
	      }
	    }
	    break;
	case Jvm_JavaDump:
	case Jvm_SystemDump:
	case Jvm_HeapDump:
	    printf("Dump done\n");
	    break;
	case Jvm_JavaLockMonitorStart:
	    printf("Started\n");
	    break;
	case Jvm_JavaLockMonitorEnd:
	    printf("Ended\n");
	    break;
	case Jvm_JavaLockMonitorDump:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_JavaLockMonitorTable_t * t = (Jvm_JavaLockMonitorTable_t *)u.data();
	      printf("monitors = %d\n", t->count);
	      Jvm_javaLockMonitor_t * table = (Jvm_javaLockMonitor_t *)u.getArray(t->jlm_table);
	      printf("Monitor_Java\n");
	      for (int i=0;i<t->count;i++)
	      {
		Jvm_javaLockMonitor_t * entry = table+i;
		if (entry->type == Jvm_Monitor_Java)
		  dump_monitor(entry, u2c(u.getUnicodeObject(t->monitor_names, i)));
	      }
	      printf("Monitor_Raw\n");
	      for (int i=0;i<t->count;i++)
	      {
		Jvm_javaLockMonitor_t * entry = table+i;
		if (entry->type == Jvm_Monitor_Raw)
		  dump_monitor(entry, u2c(u.getUnicodeObject(t->monitor_names, i)));
	      }
	    }
	    break;
	case Jvm_ForceGc:
	    printf("ForceGC done\n");
	case Jvm_setGcVerbose:
	    printf("Set done\n");
	    break;
	case Jvm_setGcSoftMax:
	    break;
	case Jvm_GetOwnedMonitorInfo:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_ThreadLockInfo_t * t = (Jvm_ThreadLockInfo_t *)u.data();
	      printf("locks	= %d\n", t->count);
	      int32 * hashCodes = (int32 *)u.getArray(t->hashCodes);
	      for (int i=0;i<t->count;i++)
	      {
		printf("hashCode = %08X class = %s", hashCodes[i], u2c(u.getUnicodeObject(t->class_names, i)));
		printf("(%s)\n", u2c(u.getUnicodeObject(t->source_files, i)));
	      }
	    }
	    break;
	case Jvm_GetCurrentContendedMonitor:
	    {
	      JvmUnpacker u(buffer);
	      Jvm_ThreadContendedLockInfo_t * t = (Jvm_ThreadContendedLockInfo_t *)u.data();
	      if (t->isContended == Jvm_FALSE)
		printf("No object contended\n");
	      else
	      {
		printf("hashCode = %08X class = %s", t->hashCode, u2c(u.getUnicodeObject(t->class_name)));
		printf("(%s)\n", u2c(u.getUnicodeObject(t->source_file)));
		printf("  holder = %06llx\n", t->holder_ktid);
	      }

	    }
	    break;
	/*case Jvm_CommandLine:
	    break;*/
	default:
	    printf("command not supported\n");
    }
  }
  else
    printf("Function Failed\n");

}

static int auto_index = 0;

Jvm_Command_t auto_input()
{
  switch (auto_index++ % 18)
  {
      case 0: return Jvm_Alive;
      case 1: return Jvm_hello;
      case 2: return Jvm_ILEenv;
      case 3: return Jvm_env;
      case 4: return Jvm_getSystemProperties;
      case 5: return Jvm_GetCurrentProperties;
      case 6: return Jvm_ext;
      case 7: return Jvm_threads;
      case 8: return Jvm_ClassLoadingMXBean;
      case 9:return Jvm_CompilationMXBean;
      case 10:return Jvm_ThreadMXBean;
      case 11:return Jvm_GarbageCollectorInfo;
      case 12:return Jvm_getMemoryInfo;
      case 13:return Jvm_GetGCCycleTable;
      case 14:return Jvm_JavaLockMonitorStart;
      case 15:return Jvm_JavaLockMonitorEnd;
      case 16:return Jvm_JavaLockMonitorDump;
      case 17:return Jvm_ForceGc;
      default: exit(-1);
  }
  return Jvm_Alive;
}

int menu(int sockfd, Jvm_Command_t & command, Jvm_ErrorCode_t & errorCode, void * & buffer, size_t & len)
{
  char user_data[1024];
  Jvm_ktid_format_t parm;
  Jvm_methodId_t parm1;
  char request[20];
  char flag[20];
  uint64 user_number;
  char * cbuffer = (char *)buffer;


  if (automode)
  {
    command = auto_input();
  }
  else
  {
    // get new command
    printf("--> ");
    fgets(user_data, sizeof(user_data), stdin);
    sscanf(user_data, "%s", request);
    printf("%s\n", request);


    if (strcmp(request, "hello") == 0)
    {
      command = Jvm_hello;
    }
    else if (strcmp(request, "help") == 0)
    {
      command = Jvm_Alive;
      printf("Commands\n");
      printf("  hello\n");
      //    printf("  sbreak\n");
      printf("  exit\n");
      printf("  env\n");
      printf("  ileenv\n");
      printf("  ti_prop\n");
      printf("  prop\n");
      printf("  ext\n");
      printf("  threads\n");
      printf("  kill\n");
      printf("  stack <os_tid>\n");
      printf("  suspend <os_tid>\n");
      printf("  resume <os_tid>\n");
      printf("  snap <os_tid>\n");
      printf("  resolve <mid>\n");
      printf("  tinfo <os_tid>\n");
      printf("  locks <os_tid>\n");
      printf("  contended <os_tid>\n");
      printf("  systemdump\n");
      printf("  javadump\n");
      printf("  heapdump\n");
      printf("  -Xdump:\n");
      printf("  sysdumpnone\n");
      printf("  javadumpnone\n");
      printf("  heapdumpnone\n");
      printf("  sig\n");
      printf("  mem\n");
      //printf("  mem2\n");
      printf("  loader\n");
      printf("  jit\n");
      printf("  gc\n");
      printf("  threading\n");
      printf("  verbose <true>\n");
      printf("  forceGC\n");
      printf("  setgcsoftmax <number>\n");
      printf("  testprint\n");
      printf("  cycle\n");
      printf("  alive\n");
      printf("  jlm_start\n");
      printf("  jlm_end\n");
      printf("  jlm_dump\n");
      printf("  auto\n");
    }
    else if (strcmp(request, "auto") == 0)
    {
      automode = 1;
      command = Jvm_Alive;
    }
    else if (strcmp(request, "alive") == 0)
    {
      command = Jvm_Alive;
    }
    else if (strcmp(request, "setgcsoftmax") == 0)
    {
      sscanf(user_data, "%s %lld", request, &user_number);
      command = Jvm_setGcSoftMax;
      Jvm_setGcSoftMax_t * data = (Jvm_setGcSoftMax_t *)buffer;
      data->gcSoftMax = user_number;
    }
    else if (strncmp(request, "-Xdump:",7) == 0)
    {
      char * real_data = request+7;
      command = Jvm_setDump;
      printf("command = %s\n", real_data);
      strcpy((char *)buffer, real_data);
    }
    /*  else if (strcmp(request, "sbreak") == 0)
     {
     command = Jvm_sbreak;
     }*/
    else if (strcmp(request, "mem") == 0)
    {
      command = Jvm_getMemoryInfo;
    }
    else if (strcmp(request, "jlm_start") == 0)
    {
      command = Jvm_JavaLockMonitorStart;
    }
    else if (strcmp(request, "jlm_end") == 0)
    {
      command = Jvm_JavaLockMonitorEnd;
    }
    else if (strcmp(request, "jlm_dump") == 0)
    {
      command = Jvm_JavaLockMonitorDump;
    }
    else if (strcmp(request, "cycle") == 0)
    {
      command = Jvm_GetGCCycleTable;
    }
    /*else if (strcmp(request, "lastgc") == 0)
     {
     command = Jvm_LastGCInfo;
     }*/
    else if (strcmp(request, "forceGC") == 0)
    {
      command = Jvm_ForceGc;
    }
    else if (strcmp(request, "gc") == 0)
    {
      command = Jvm_GarbageCollectorInfo;
    }
    else if (strcmp(request, "loader") == 0)
    {
      command = Jvm_ClassLoadingMXBean;
    }
    else if (strcmp(request, "jit") == 0)
    {
      command = Jvm_CompilationMXBean;
    }
    else if (strcmp(request, "threading") == 0)
    {
      command = Jvm_ThreadMXBean;
    }
    else if (strcmp(request, "systemdump") == 0)
    {
      command = Jvm_SystemDump;
    }
    else if (strcmp(request, "javadump") == 0)
    {
      command = Jvm_JavaDump;
    }
    else if (strcmp(request, "heapdump") == 0)
    {
      command = Jvm_HeapDump;
    }
    else if (strcmp(request, "verbose") == 0)
    {
      command = Jvm_setGcVerbose;
      Jvm_GcVerbose_t * boolean = (Jvm_GcVerbose_t *)buffer;
      sscanf(user_data, "%s %s", request, flag);
      if (strcmp(flag, "true") == 0)
      {
	printf("requesting verbose=true\n");
	boolean->verbose = Jvm_TRUE;
      }
      else
      {
	printf("requesting verbose=false\n");
	boolean->verbose = Jvm_FALSE;
      }
    }
    else if (strcmp(request, "sysdumpnone") == 0)
    {
      command = Jvm_setDump;
      strcpy(cbuffer, "system:none");
    }
    else if (strcmp(request, "javadumpnone") == 0)
    {
      command = Jvm_setDump;
      strcpy(cbuffer, "java:none");
    }
    else if (strcmp(request, "heapdumpnone") == 0)
    {
      command = Jvm_setDump;
      strcpy(cbuffer, "heap:none");
    }
    else if (strcmp(request, "sig") == 0)
    {
      command = Jvm_raiseSignal;
    }
    else if (strcmp(request, "exit") == 0)
    {
      return 0;
    }
    else if (strcmp(request, "env") == 0)
    {
      command = Jvm_env;
    }
    else if (strcmp(request, "ileenv") == 0)
    {
      command = Jvm_ILEenv;
    }
    else if (strcmp(request, "prop") == 0)
    {
      command = Jvm_GetCurrentProperties;
    }
    else if (strcmp(request, "ti_prop") == 0)
    {
      command = Jvm_getSystemProperties;
    }
    else if (strcmp(request, "ext") == 0)
    {
      command = Jvm_ext;
    }
    else if (strcmp(request, "threads") == 0)
    {
      command = Jvm_threads;
    }
    else if (strcmp(request, "kill") == 0)
    {
      command = Jvm_kill;
    }
    else if (strncmp(request, "stack", strlen("stack")) == 0)
    {
      command = Jvm_stack;
      sscanf(user_data, "%s %llx", request, &parm.ktid);
      printf("tid = %016llX\n", parm.ktid);
      memcpy(cbuffer, &parm, sizeof(parm));
    }
    else if (strncmp(request, "suspend", strlen("suspend")) == 0)
    {
      command = Jvm_suspendThread;
      sscanf(user_data, "%s %llx", request, &parm.ktid);
      printf("ktid = %016llX\n", parm.ktid);
      memcpy(cbuffer, &parm, sizeof(parm));
    }
    else if (strncmp(request, "resume", strlen("resume")) == 0)
    {
      command = Jvm_resumeThread;
      sscanf(user_data, "%s %llx", request, &parm.ktid);
      printf("ktid = %016llX\n", parm.ktid);
      memcpy(cbuffer, &parm, sizeof(parm));
    }
    else if (strncmp(request, "snap", strlen("snap")) == 0)
    {
      command = Jvm_suspendAndSnapshot;
      sscanf(user_data, "%s %llx", request, &parm.ktid);
      printf("ktid = %016llX\n", parm.ktid);
      memcpy(cbuffer, &parm, sizeof(parm));
    }
    else if (strncmp(request, "resolve", strlen("resolve")) == 0)
    {
      command = Jvm_ResolveMethod;
      sscanf(user_data, "%s %llx", request, &parm1.mid);
      printf("mid = %016llX\n", parm1.mid);
      memcpy(cbuffer, &parm1, sizeof(parm1));
    }
    else if (strncmp(request, "tinfo", strlen("tinfo")) == 0)
    {
      command = Jvm_getThreadInfo;
      sscanf(user_data, "%s %llx", request, &parm.ktid);
      printf("ktid = %016llX\n", parm.ktid);
      memcpy(cbuffer, &parm, sizeof(parm));
    }
    else if (strncmp(request, "locks", strlen("locks")) == 0)
    {
      command = Jvm_GetOwnedMonitorInfo;
      sscanf(user_data, "%s %llx", request, &parm.ktid);
      printf("ktid = %016llX\n", parm.ktid);
      memcpy(cbuffer, &parm, sizeof(parm));
    }
    else if (strncmp(request, "contended", strlen("contended")) == 0)
    {
      command = Jvm_GetCurrentContendedMonitor;
      sscanf(user_data, "%s %llx", request, &parm.ktid);
      printf("ktid = %016llX\n", parm.ktid);
      memcpy(cbuffer, &parm, sizeof(parm));
    }
    else
    {
      printf("Unknown command, sending Jvm_Alive instead\n");
      command = Jvm_Alive;
    }
  }

  server(sockfd, command, errorCode, buffer, len, 0);
  
  return 1;
}

int main(int argc, char * argv[])
{
  Jvm_Command_t command = Jvm_Init;
  Jvm_ErrorCode_t errorCode;
  void * buffer;
  size_t len;
  int port = 6543;

  if (argc < 2)
  {
    printf("usage: client_menu <system_name> <port>\n");
  }
  char * system_name = argv[1];
  if (argc == 3)
  {
    port = atoi(argv[2]);
  }
  int sockfd = setup(system_name, port);
  
  recv_command(sockfd, command, errorCode, buffer, len);	// ignore
  print_data(sockfd, command,  errorCode, buffer, len);

  int rc = 1;
  while (rc)
  {
    rc = menu(sockfd, command, errorCode, buffer, len);
    
    if (rc)
      print_data(sockfd, command,  errorCode, buffer, len);

  }
  close(sockfd);
  exit(0);
}

