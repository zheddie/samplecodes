#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include <sys/limits.h>
#include "zgdebug.h"

#define PS_ADD		0x0
#define PS_MOD		0x1
#define PS_DELETE	0x2

typedef int pollset_t;

struct poll_ctl {
	short cmd;
	short events;
	int fd;
};

#define NFDS(x)		((x) & 0x0000FFFF)

typedef enum{
	MODFIND=0,
	MODSHRINK
} SEEKMODE;
typedef enum{
	FLGCHANGED=0,
	FLGUNCHANGED
} CHGFLG;

typedef struct pollset_data{
	/*offset: 0x00*/
	pollset_t psid;				/*pollset ID */
	/*offset: 0x08  , this is not good!!*/
	int32 pfds_max;				/*Max len of pollfd, input from application.*/
	int32 pfds_len;				/*length of the pollfd*/
	/*offset: 0x10*/
	struct pollfd * pfds;		/*pollfd to be polled.*/
	/*offset: 0x18*/
	struct pollset_data * next;	/*link to the next pollset*/
	/*This address was saved incase nothing changed while the size is too big, to avoid malloc too many times.*/
	/*offset: 0x20*/
	struct pollfd * tobepoll;	
	/*offset: 0x28*/
	pid_t pid;				/*The process which owned this psid.*/
	/*offset: 0x30	*/
	CHGFLG changeflag;
	pthread_mutex_t item_lock;
} POLLSET_DATA;

#define FDBLOCKSIZE	1000

#define BLOCKCOUNT(ps) (ps->pfds_len/FDBLOCKSIZE+((ps->pfds_len%FDBLOCKSIZE == 0)?0:1))
#define LENINBLOCK(ps,idx) ((idx == BLOCKCOUNT(ps)-1 && ps->pfds_len%FDBLOCKSIZE!=0)? ps->pfds_len%FDBLOCKSIZE:FDBLOCKSIZE)
#define LENINLASTBLOCK(ps) ((ps->pfds_len%FDBLOCKSIZE!=0 )? ps->pfds_len%FDBLOCKSIZE:FDBLOCKSIZE)


static POLLSET_DATA * _pollsetlist = NULL;
static POLLSET_DATA * _last = NULL;
/*static int32 ps_len = 0;*/

pthread_once_t init_once = PTHREAD_ONCE_INIT;
pthread_mutex_t list_lock;

void do_init(void)
{
	/*We wish to do some kind of initialization here*/
	pthread_mutex_init(&list_lock, NULL);
	return;
}

int init_locks(void)
{

	pthread_once(&init_once, do_init);
	return (0);
}



pollset_t pollset_create(int maxfd){
	/*printf("zg.POLLSET.SO,pollset_create\n");*/
	POLLSET_DATA * one = _pollsetlist;
	int32 rt  = 0;

	/*Try to find one existing unused item firstly.*/
	if (NULL == one){
		rt = init_locks();
		if(0!= rt){
			return(rt);
		}
	}else{
		while(one != NULL){
			if(one -> pid == -1){
				break;
			}
			one = one->next;
		}
	}
	if(one == NULL){	/*All existing psid is in use. Need create new one.*/
		one = (POLLSET_DATA *)malloc(sizeof(POLLSET_DATA));
		if(NULL == one){
			rt = -1;
			errno = ENOMEM;
			return(rt);
		}
		/*one = (POLLSET_DATA *)malloc(sizeof(POLLSET_DATA));*/
		else{
			
			/*do we need lock here?*/
			if(pthread_mutex_init(&(one->item_lock), NULL) != 0){
				return(-1);
			}
			pthread_mutex_lock(&(one->item_lock));
			/*Make sure these initialized before psid, as psid could be query by other routines.*/
			one->pid = getpid();
			one->pfds_len = 0;
			one->pfds_max  = maxfd;
			one->pfds = NULL;
			one->changeflag = FLGCHANGED;
			one->tobepoll = NULL;
			pthread_mutex_lock(&list_lock);
			/*Try to add this new item into the list.*/
			if(NULL != _last){
				_last ->next = one;
				one->psid  = _last->psid + 1;
				/*ps_len ++;*/
			}else{/* If _last is NULL, then assume the list is empty.*/
				_pollsetlist = one;
				/*ps_len = 1;*/
				one->psid  = 1;   /*let's start from 1 of the psid.*/
			}
			_last = one;
			/*For the new created item, we set the next to NULL.*/
			one->next = NULL;
			pthread_mutex_unlock(&list_lock);
			pthread_mutex_unlock(&(one->item_lock));
		}			
	}
	rt = one->psid;
#ifdef POLLSET_DEBUG
	printf("EXIT.pollset_create(),return:%d,one->psid=%d.\n",rt,one->psid);
#endif
	return rt;
}
/*Get the next available slot of pollfd.*/
/*New block could be generated if no space available within max.*/

int32 getnext(POLLSET_DATA *ps,struct pollfd **p){


	struct pollfd * pf = NULL, * pfnew= NULL, *ptemp = NULL;
	int32 rt =0;

	if(ps->pfds_len < ps->pfds_max){ 		/*We haven't hit the max , have extra space.*/

		int blockCount = 0;
		pf = ps->pfds;
		int noneedcreatenew = ps->pfds_len%FDBLOCKSIZE;
		blockCount = BLOCKCOUNT(ps);
		for(int i =0;i< blockCount -1;i++ ){
			memcpy((void *)&pf,(void *)&pf[FDBLOCKSIZE],sizeof(struct pollfd *));
		}

		int leninblock = LENINLASTBLOCK(ps);
		if(!noneedcreatenew) { 			/*No space in current block, need create a new space.*/
			pfnew = ( struct pollfd *) malloc((FDBLOCKSIZE+1) * sizeof(struct pollfd));
			if(pfnew != NULL){

				memset((void *)pfnew,0,sizeof(struct pollfd)*(FDBLOCKSIZE+1));
				if(NULL !=pf){
					memcpy((void*)&pf[FDBLOCKSIZE],(void *)&pfnew,sizeof( struct pollfd *));	/*write the next pfds into the last element in previous pfds.*/
				}else{	/*pf is NULL, means this is the frist time to create any entry in this pollset.*/
					ps->pfds = pfnew;
				}
				pf = pfnew;
				leninblock  = 0;
			}else{	/*No enough memory.*/
				rt = -1;
				errno = ENOMEM;
				return(rt);
			}
		}
		*p = &pf[leninblock];
	}else{
		rt = -1;
		errno = EMFILE;
		*p = NULL;
	}

	return (rt);
}

struct pollfd *  findfd(POLLSET_DATA *  ps,int fd){

	int blockCount = BLOCKCOUNT(ps);
	struct pollfd * p = NULL,*pfl = ps->pfds;

	for(int i =0;i< blockCount;i++ ){
		if(i>0){
			memcpy((void *)&pfl,(void *)&pfl[FDBLOCKSIZE],sizeof(struct pollfd*));
		}
		if(NULL != pfl ){
			int leninblock = LENINBLOCK(ps,i);
			for(int j=0;j<leninblock;j++){
				if(pfl[j].fd == fd){	/*Found the target fd to control.*/
					p = &(pfl[j]);
					break;
				}
			}
			if(NULL != p){
				break;
			}
		}else{
			break;
		}
	}
	return(p);
}
int32 addfd(POLLSET_DATA * ps,struct poll_ctl *pc){
	int rt = 0;
	struct pollfd * p = findfd(ps,pc->fd);
	if( NULL !=p ){	/*oops, error?*/
		errno = EEXIST;
		rt = -1;
		return(rt);
	}else{
		rt = getnext(ps,&p);
		if(0 != rt){
			errno = ENOMEM;
			return(rt);
		}
		p->fd = pc->fd;
		p->events = pc->events;
		p->revents = 0;
		ps->pfds_len++;
		ps->changeflag = FLGCHANGED;
	}
	return(rt);
}

int32 deletefd(POLLSET_DATA * ps,int fd){

	int blockCount = BLOCKCOUNT(ps);
	struct pollfd * p = NULL;
	int idx = -1,rt = 0;
	struct pollfd *pfl = ps->pfds;
	struct pollfd * pfl_p = pfl;
	SEEKMODE sm = MODFIND;

	for(int i =0;i< blockCount;i++ ){
		if(i>0){
			pfl_p = pfl;
			memcpy((void *)&pfl,(void *)&pfl[FDBLOCKSIZE],sizeof(struct pollfd*));

		}
		if(NULL != pfl ){
			int leninblock = LENINBLOCK(ps,i);
			for(int j=0;j<leninblock;j++){
				if(MODFIND == sm){
					if(pfl[j].fd == fd){	/*Found the target fd to control.*/
						sm = MODSHRINK;
					}
				}else{/*In shrink mode.*/
					if(0==j){
						pfl_p[FDBLOCKSIZE - 1] = pfl[j];
						if(1==leninblock){	/*There's only one item in this block, which is moved to previous block. Try to release this block.*/
							free(pfl);
							if( pfl_p  == pfl){
								ps->pfds  = NULL;
							}else{
								memset(&pfl_p[FDBLOCKSIZE],0,sizeof(struct pollfd));
							}

						}
					}else{
						pfl[j-1] = pfl[j];
					}
				}
			}
		}else{	/*Should not hit here.*/
			errno = EINVAL;
			rt = -1;
			break;
		}
	}
	if(MODSHRINK == sm){
		ps->pfds_len --;
		ps->changeflag = FLGCHANGED;
	}else{
		errno = EINVAL;
		rt = -1;
	}
	return(rt);

}
int32 modfd(POLLSET_DATA * ps,struct poll_ctl *pc){
	int rt = 0;
	struct pollfd * p = findfd(ps,pc->fd);
	if( NULL ==p ){	/*oops, error?*/
		errno = EINVAL;
		rt = -1;
		return(rt);
	}else{
		p->events |= pc->events;
		ps->changeflag = FLGCHANGED;
	}
	return(rt);
}
int pollset_ctl(pollset_t ps, struct poll_ctl *pollctl_array,
		       int array_length){
#ifdef POLLSET_DEBUG
	printf("ENTRY.pollset_ctl(ps=%d,array_length=%d)\n",ps,array_length);
#endif
	POLLSET_DATA * header = _pollsetlist;
	int32 rt = 0;
	while(header != NULL){
		if(header->psid == ps){
			if(getpid() != header->pid){
				errno = EACCES;
				rt = -1;
				return(rt);
			}
			break;
		}
		header = header->next;
	}
	if(NULL != header){
	
		for(int i =0;i<array_length;i++){
			switch(pollctl_array[i].cmd){
			case PS_ADD:
				pthread_mutex_lock(&(header->item_lock));
				rt = addfd(header,&pollctl_array[i]);
				pthread_mutex_unlock(&(header->item_lock));
				if(0!=rt){
					return (rt);
				}
				break;
			case PS_MOD:
				pthread_mutex_lock(&(header->item_lock));
				rt = modfd(header,&pollctl_array[i]);
				pthread_mutex_unlock(&(header->item_lock));
				if(0!=rt){
					return (rt);
				}
				break;
			case PS_DELETE:
				pthread_mutex_lock(&(header->item_lock));
				rt = deletefd(header,pollctl_array[i].fd);
				pthread_mutex_unlock(&(header->item_lock));
				if(0!=rt){
					return (rt);
				}
				break;
			default:
				errno = EINVAL;
				rt = -1;
				return(rt);
			}
		}
	}else{
		rt = -1;
		errno = EINVAL;
	}
#ifdef POLLSET_DEBUG
	printf("EXIT.pollset_ctl(),return:%d\n",rt);
#endif
	return (rt);
}
POLLSET_DATA * findps(pollset_t psid){
	POLLSET_DATA * header = _pollsetlist;
	while(header != NULL){
		if(header->psid == psid){
			break;
		}
		header = header->next;
	}
	return header;
}



int pollset_destroy(pollset_t ps){
#ifdef POLLSET_DEBUG
	printf("ENTRY.pollset_query(ps=%d)\n",ps);
#endif
	POLLSET_DATA * header =findps(ps);
	int32 rt = 0;
	if(header != NULL){
		if(getpid() != header->pid){
			errno = EACCES;
			rt = -1;
			return(rt);
		}
		pthread_mutex_lock(&(header->item_lock));
		if (header->pid != -1){ /* in case thread racing condition*/
			int blockCount = BLOCKCOUNT(header);
			struct pollfd * pfl = header->pfds, *pfl_p = header->pfds;

			for(int i =0;i< blockCount;i++ ){
				if(NULL != pfl_p){
					memcpy((void *)&pfl,(void *)&pfl_p[FDBLOCKSIZE],sizeof(struct pollfd*));
					free(pfl_p);
					pfl_p = pfl;
				}else{
					break;
				}
			}
			if(header->tobepoll != NULL){
				free(header->tobepoll);
				header->tobepoll = NULL;

			}
			/*We expect pfl_p for the last one is NULL.*/
			/*TODO: here should be a assersion of pfl_p for NULL.*/

			/*We do not want to delete header, instead , keep it there for furture use.*/
			/*free(header);*/
			/*header = NULL;*/
			header->pfds_max = 0;
			header->pfds = NULL;
			header->changeflag = FLGCHANGED;
			header->pfds_len = 0;
			header->pid = -1;	/*This is a indication of an un-used item.*/
		}
		pthread_mutex_unlock(&(header->item_lock));
		rt = 0;
		errno=0;

	}else{
		errno = ENOENT;
		rt=-1;
	}
#ifdef POLLSET_DEBUG
	printf("EXIT.pollset_destroy(),return:%d\n",rt);
#endif
	return(rt);
}
int pollset_query(pollset_t psid,struct pollfd *querydata){
#ifdef POLLSET_DEBUG
	printf("ENTRY.pollset_query(psid=%d)\n",psid);
#endif
	POLLSET_DATA * psp = findps(psid);
	struct pollfd *  pfd = NULL;
	int32 rt = 0;
	if(NULL != psp){
		if(getpid() != psp->pid){
			errno = EACCES;
			rt = -1;

		}else{
			pfd = findfd(psp,querydata->fd);
			if(NULL != pfd){
				querydata->events = pfd->events;
				errno = 0;
				rt = 1;
			}else{
				errno = ENOENT;
				rt = -1;
			}
		}

	}else{
		errno = EACCES;
		rt = -1;

	}
#ifdef POLLSET_DEBUG
	printf("EXIT.pollset_query(),return:%d\n",rt);
#endif
	return(rt);

}
/*This method is used to get a continues*/
int getpollfd(POLLSET_DATA * psp,struct pollfd **pfds,int *len)
{
	int rt = 0;

	*pfds = NULL; *len = 0;
	if(NULL != psp){
		if(getpid() != psp->pid){
			errno = EACCES;
			rt = -1;
			return(rt);
		}
		if(psp->pfds_len <= FDBLOCKSIZE ){ /*We would assume this is the most situation.*/
			*len = psp->pfds_len;
			*pfds = psp->pfds;
		}else{	/*bigger than FDBLOCKSIZE, need new buffer. This is assumed to be very few occurrence.*/
			if(psp->changeflag == FLGCHANGED){	/*If application code is in look on pollset, they would not hit this branch. This would get better performance.*/
				if(NULL != psp->tobepoll){
					free(psp->tobepoll);
					psp->tobepoll = NULL;
				}
				struct pollfd * ptemp = (struct pollfd *)malloc(psp->pfds_len*sizeof(struct pollfd));

				if(NULL != ptemp){
					memset((void *)ptemp,0,psp->pfds_len*sizeof(struct pollfd));

					struct pollfd *pfl = psp->pfds;
					int idx = 0;
					int blockCount = BLOCKCOUNT(psp);
					for(int j = 0;j<blockCount;j++){
						int leninblock = LENINBLOCK(psp,j);
						memcpy((void *)&ptemp[idx],(void *)&pfl[0],leninblock*sizeof(struct pollfd));
						idx += leninblock;
						/*Move to next block*/
						if( j < blockCount - 1){
							memcpy((void *)&pfl,(void *)&pfl[FDBLOCKSIZE],sizeof(struct pollfd*));
						}
					}
					psp->tobepoll = ptemp;
					psp->changeflag = FLGUNCHANGED;
				}else{
					errno = ENOMEM;
					rt=-1;
					return(rt);
				}
			}
			*pfds = psp->tobepoll;
			*len = psp->pfds_len;
		}
		rt= 0;
		errno = 0;

	}else{
		rt = -1;
		errno = EINVAL;
	}
	return (rt);
}
int pollset_poll(pollset_t ps, struct pollfd *polldata_array,
			int array_length, int timeout){
	struct pollfd * pf = NULL;
	int pflen = 0;
	int rt = -1;
#ifdef POLLSET_DEBUG
	printf("ENTRY.pollset_poll(ps=%d,array_length=%d,timeout=%d)\n",ps,array_length,timeout);
#endif
	POLLSET_DATA * psp = findps(ps);
	if(psp != NULL){
		pthread_mutex_lock(&(psp->item_lock));
		/*printf("zg.POLLSET.SO,pollset_poll\n");*/
		rt = getpollfd(psp,&pf,&pflen);

		if(0==rt && pflen>0){
			rt = poll(pf, pflen, timeout);
		    if (rt >= 0)
		    {
				int idx  = 0;
				int totallen = NFDS(rt)>array_length?array_length:NFDS(rt);
				for(int i =0;i< pflen;i++){
					if(pf[i].revents == 0){
						continue;
					}else{
						if(idx< totallen){
							polldata_array[idx].fd = pf[i].fd;	/*Avoid converting again.*/
							polldata_array[idx].events = pf[i].events;
							polldata_array[idx].revents = pf[i].revents;
							idx++;
						}else{	/*No extra space to store the polled data.*/
							errno = ENOSPC;
							break;
						}
					}
				}
				pthread_mutex_unlock(&(psp->item_lock));
				return(idx);
			}
		}
		pthread_mutex_unlock(&(psp->item_lock));
	}
	errno = EFAULT;
#ifdef POLLSET_DEBUG
	printf("EXIT.pollset_poll(),return:%d\n",rt);
#endif
	return(rt);
}
