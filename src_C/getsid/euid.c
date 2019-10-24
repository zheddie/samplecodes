/*
 * euid.c
 *
 *  Created on: Sep 23, 2016
 *      Author: zhanggan
 */
#include <unistd.h>
#include <stdio.h>
int main(){
	pid_t uid = getuid();
	pid_t euid = geteuid();
	int rt = seteuid(euid);
	printf("uid=%d,euid=%d,rt=%d\n",uid,euid,rt);
}

