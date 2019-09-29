/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* bos720 src/bos/kernel/sys/pollset.h 1.4                                */
/*                                                                        */
/* Licensed Materials - Property of IBM                                   */
/*                                                                        */
/* Restricted Materials of IBM                                            */
/*                                                                        */
/* COPYRIGHT International Business Machines Corp. 2005,2009              */
/* All Rights Reserved                                                    */
/*                                                                        */
/* US Government Users Restricted Rights - Use, duplication or            */
/* disclosure restricted by GSA ADP Schedule Contract with IBM Corp.      */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
/* @(#)70	1.4  src/bos/kernel/sys/pollset.h, sysios, bos720 2/16/09 11:28:36 */
#ifndef _H_POLLSET
#define _H_POLLSET
#ifndef _H_POLL
#include <sys/poll.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PS_ADD		0x0
#define PS_MOD		0x1
#define PS_DELETE	0x2

typedef int pollset_t;

struct pollingset {
	struct pollfd	*ps_fds;
	int		array_length;
	int		timeout;
};
typedef struct pollingset pollingset_t;

struct poll_ctl {
	short cmd;
	short events;
	int fd;
};

extern pollset_t pollset_create(int maxfd);
extern int pollset_destroy(pollset_t ps);
extern int pollset_query(pollset_t ps, struct pollfd *pollfd_query);
extern int pollset_ctl(pollset_t ps, struct poll_ctl *pollctl_array,
		       int array_length);
extern int pollset_poll(pollset_t ps, struct pollfd *polldata_array,
			int array_length, int timeout);
extern int pollset_getcache(void *obuf, int * len);
extern int pollset_putcache(void *obuf, int len);


#ifdef __cplusplus
}
#endif

#ifdef _KERNSYS

typedef struct pollingset64 {
	int		ps_fds_hi;
	int		ps_fds_lo;
	int		array_length;
	int		timeout;
}pollingset64_t;

#endif
#endif	/* _H_POLLSET */
