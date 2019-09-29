/*
 * PasePollset.H
 *
 *  Created on: Jun 12, 2017
 *      Author: zhanggan
 */

#ifndef PASE_FILESYSTEM_IFS_PASEPOLLSET_H_
#define PASE_FILESYSTEM_IFS_PASEPOLLSET_H_

#include <poll.h>
#include <unistd.h>
#include "pollset.h"


#ifdef DEBUG
#include <stdio.h>
#define PRINTF(fmt,...) printf(fmt,##__VA_ARGS__)
#else
#define PRINTF(fmt,...)
#endif

#define int32 int
#define int64 int32
//#define pollset_t int
//#define PS_ADD		0x0
//#define PS_MOD		0x1
//#define PS_DELETE	0x2

#define pid_t_aix pid_t

#define FDBLOCKSIZE	100

#define BLOCKCOUNT(ps) (ps->pfds_len/FDBLOCKSIZE+((ps->pfds_len%FDBLOCKSIZE == 0)?0:1))
#define LENINBLOCK(ps,idx) ((idx == BLOCKCOUNT(ps)-1 && ps->pfds_len%FDBLOCKSIZE!=0)? ps->pfds_len%FDBLOCKSIZE:FDBLOCKSIZE)
#define LENINLASTBLOCK(ps) ((ps->pfds_len%FDBLOCKSIZE!=0 )? ps->pfds_len%FDBLOCKSIZE:FDBLOCKSIZE)

typedef enum{
	MODFIND=0,
	MODSHRINK
} SEEKMODE;
typedef enum{
	FLGCHANGED=0,
	FLGUNCHANGED
} CHGFLG;

typedef struct pollset_data{
	pollset_t psid;				//pollset ID 
	int32 pfds_max;				//Max len of pollfd, input from application.
	struct pollfd * pfds;		//pollfd to be polled.
	struct pollset_data * next;	//link to the next pollset
	//This address was saved incase nothing changed while the size is too big, to avoid malloc too many times.
	struct pollfd * tobepoll;		
	CHGFLG changeflag;
	int32 pfds_len;				//length of the pollfd
	//int32 pfds_rlen;			//Reached len, including the deleted fds.
	pid_t_aix pid;				//The process which owned this psid.
	
	
} POLLSET_DATA;

class PasePollset {
public:
	static PasePollset pps ;		//This is a global variable, which is used to hold the pollset data.

	PasePollset();
	pollset_t createpollset(int32);
	int32 controlpollset(pollset_t , struct poll_ctl *,int64 );
	int32 destroypollset(pollset_t);
	int32 querypollset(pollset_t,struct pollfd *);
	
	int32 getpollfd(pollset_t ,struct pollfd **,int *);
	
	struct pollfd * findfd(POLLSET_DATA *  ,int);
	POLLSET_DATA * findps(pollset_t  );
	int32 addfd(POLLSET_DATA *  ,struct poll_ctl *);
	int32 deletefd(POLLSET_DATA *  , int);
	int32 modfd(POLLSET_DATA *  ,struct poll_ctl *);
	int32 getnext(POLLSET_DATA *,struct pollfd **);
	
	virtual ~PasePollset();
	
private:

	POLLSET_DATA * first;
	POLLSET_DATA * last;
	int32 ps_len;
};

#endif /* PASE_FILESYSTEM_IFS_PASEPOLLSET_H_ */
