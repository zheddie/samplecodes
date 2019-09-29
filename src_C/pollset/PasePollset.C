/*
 * PasePollset.C
 *
 *  Created on: Jun 12, 2017
 *      Author: zhanggan
 */

#include "PasePollset.H"
#include <string.h>
#include <errno.h>
#include <stdlib.h>


//#define NULL 0

PasePollset::PasePollset() {
	ps_len = 0;
	first = NULL;
	last = NULL;

};

PasePollset::~PasePollset() {
	// TODO Auto-generated destructor stub
}

POLLSET_DATA * PasePollset::findps(pollset_t psid){
	POLLSET_DATA * header = first;
	while(header != NULL){
		if(header->psid == psid){
			break;
		}
		header = header->next;
	}
	return header;
}
struct pollfd *  PasePollset::findfd(POLLSET_DATA *  ps,int fd){

	int blockCount = BLOCKCOUNT(ps);
	struct pollfd * p = NULL,*pfl = ps->pfds;

	for(int i =0;i< blockCount;i++ ){
		if(i>0){
			memcpy((void *)&pfl,(void *)&pfl[FDBLOCKSIZE],sizeof(struct pollfd*));
		}
		if(NULL != pfl ){
			int leninblock = LENINBLOCK(ps,i);
			for(int j=0;j<leninblock;j++){
				if(pfl[j].fd == fd){	//Found the target fd to control.
					p = &(pfl[j]);
					break;
				}
			}
			if(NULL != p){
				break;
			}
		}else{//Should not reach here.Anyway, return NULL;
			break;
		}
	}
	return(p);
}
//Get the next available slot of pollfd.
//New block could be generated if no space available within max.

int32 PasePollset::getnext(POLLSET_DATA *ps,struct pollfd **p){


	struct pollfd * pf = NULL, * pfnew= NULL, *ptemp = NULL;
	int32 rt =0;

	if(ps->pfds_len < ps->pfds_max){ 		//We haven't hit the max , have extra space.

		int blockCount = 0;
		pf = ps->pfds;
		int noneedcreatenew = ps->pfds_len%FDBLOCKSIZE;
		blockCount = BLOCKCOUNT(ps);
		for(int i =0;i< blockCount -1;i++ ){
			memcpy((void *)&pf,(void *)&pf[FDBLOCKSIZE],sizeof(struct pollfd *));
		}

		int leninblock = LENINLASTBLOCK(ps);
		if(!noneedcreatenew) { 			//No space in current block, need create a new space.
			pfnew = ( struct pollfd *) malloc((FDBLOCKSIZE+1) * sizeof(struct pollfd));
			if(pfnew != NULL){

				memset((void *)pfnew,0,sizeof(struct pollfd)*(FDBLOCKSIZE+1));
				if(NULL !=pf){
					memcpy((void*)&pf[FDBLOCKSIZE],(void *)&pfnew,sizeof( struct pollfd *));	//write the next pfds into the last element in previous pfds.
				}else{	//pf is NULL, means this is the frist time to create any entry in this pollset.
					ps->pfds = pfnew;
				}
				pf = pfnew;
				leninblock  = 0;
			}else{	//No enough memory.
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
int32 PasePollset::addfd(POLLSET_DATA * ps,struct poll_ctl *pc){
	int rt = 0;
	struct pollfd * p = findfd(ps,pc->fd);
	if( NULL !=p ){	//oops, error?
		errno = EEXIST;
		rt = -1;
		return(rt);
	}else{
		rt = getnext(ps,&p);
		if(0 != rt){
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

int32 PasePollset::deletefd(POLLSET_DATA * ps,int fd){

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
					if(pfl[j].fd == fd){	//Found the target fd to control.
						sm = MODSHRINK;
					}
				}else{//In shrink mode.
					if(0==j){
						pfl_p[FDBLOCKSIZE - 1] = pfl[j];
						if(1==leninblock){	//There's only one item in this block, which is moved to previous block. Try to release this block.
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
		}else{	//Should not hit here.
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
int32 PasePollset::modfd(POLLSET_DATA * ps,struct poll_ctl *pc){
	int rt = 0;
	struct pollfd * p = findfd(ps,pc->fd);
	if( NULL ==p ){	//oops, error?
		errno = EINVAL;
		rt = -1;
		return(rt);
	}else{
		p->events |= pc->events;
		ps->changeflag = FLGCHANGED;
	}
	return(rt);
}
//This method is used to get a continues
int32 PasePollset::getpollfd(pollset_t psid,struct pollfd **pfds,int *len)
{
	int rt = 0;

	*pfds = NULL; *len = 0;
	POLLSET_DATA * psp = findps(psid);

	if(NULL != psp){
		int blockCount = BLOCKCOUNT(psp);
		PRINTF("----------psp->pfds_len=%d,blockCount =%d ----\n",psp->pfds_len,blockCount);
		//if(PpPaseProcess::getPaseProcessPtr()->getPID != psp->pid){
		if(getpid() != psp->pid){
			errno = EACCES;
			rt = -1;
			return(rt);
		}
		if(psp->pfds_len <= FDBLOCKSIZE ){ //We would assume this is the most situation.
			*len = psp->pfds_len;
			*pfds = psp->pfds;

		}else{	//bigger than FDBLOCKSIZE, need new buffer. This is assumed to be very few occurrence.
			PRINTF("zg.psp->pfds_len=%d\n",psp->pfds_len);
			if(psp->changeflag == FLGCHANGED){	//If application code is in look on pollset, they would not hit this branch. This would get better performance.
				if(NULL != psp->tobepoll){
					free(psp->tobepoll);
					psp->tobepoll = NULL;
				}
				struct pollfd * ptemp = (struct pollfd *)malloc(psp->pfds_len*sizeof(struct pollfd));

				if(NULL != ptemp){
					memset((void *)ptemp,0,psp->pfds_len*sizeof(struct pollfd));

					struct pollfd *pfl = psp->pfds;
					int idx = 0;
					for(int j = 0;j<blockCount;j++){
						int leninblock = LENINBLOCK(psp,j);
						memcpy((void *)&ptemp[idx],(void *)&pfl[0],leninblock*sizeof(struct pollfd));
						idx += leninblock;
						//Move to next block
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
			rt= 0;
			errno = 0;
		}
	}else{
		rt = -1;
		errno = EINVAL;
	}
	return (rt);
}
int32 PasePollset::destroypollset(pollset_t ps){
	POLLSET_DATA * header = first,*prev = NULL;
	int32 rt = 0;
	while(header != NULL){
		if(header->psid == ps){
			if(getpid() != header->pid){
				errno = EACCES;
				rt = -1;
				return(rt);
			}
			if(NULL != prev){
					prev->next = header->next;
					if(header == last) last = prev;
			}else{
				first = header->next;
				if(header == last) last = NULL;
			}
			break;
		}
		prev = header;
		header = header->next;
	}

	if(header != NULL){
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
		free(header);
		header = NULL;
		rt = 0;
		errno=0;
	}else{
		errno = ENOENT;
		rt=-1;
	}
	return(rt);
}
int32 PasePollset::querypollset(pollset_t psid,struct pollfd *querydata){
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
	return(rt);

}
int32 PasePollset::controlpollset(pollset_t ps, struct poll_ctl *pollctl_array,
	        int64 array_length){
	POLLSET_DATA * header = first;
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
				rt = addfd(header,&pollctl_array[i]);
				if(0!=rt){
					return (rt);
				}
				break;
			case PS_MOD:
				rt = modfd(header,&pollctl_array[i]);
				if(0!=rt){
					return (rt);
				}
				break;
			case PS_DELETE:
				rt = deletefd(header,pollctl_array[i].fd);
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
	return (rt);
}

pollset_t PasePollset::createpollset(int32 max){
	POLLSET_DATA * one = NULL;
	struct pollfd * pf  = NULL;
	int32 rt = 0;
	short allocationfail = 0;
	one = (POLLSET_DATA *)malloc(sizeof(POLLSET_DATA));
	if (NULL != one){

		one->pid = getpid();
		one->pfds_len = 0;
		one->pfds_max  = max;
		one->pfds = NULL;
		one->next = NULL;
//		rt = getnext(one,&pf);	//Get the first entry when create. This would create the first block.
//		if( 0 != rt){
//			free(one);
//			one  = NULL;
//			return(rt);
//		}else{
//			one->pfds = pf;
//		}
		if(NULL != last){
			last ->next = one;
			one->psid  = last->psid + 1;
			ps_len ++;
		}else{// If last is NULL, then assume the list is empty.
			first = one;
			ps_len = 1;
			one->psid  = 1;   //let's start from 1 of the psid.
		}
		last = one;
		one->changeflag = FLGUNCHANGED;
		one->tobepoll = one->pfds;
		rt = one->psid;
	}else{
		rt = -1;
		errno = ENOMEM;
	}
	return rt;
}
