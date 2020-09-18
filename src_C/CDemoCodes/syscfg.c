/***************************************************************************
*
* IBM(China),Inc. All Rights Reserved. Copyright (c) 2014
*
***************************************************************************/

/**
* @Author: qinglwbj@cn.ibm.com
* @Last modified: 2014-04-09 00:20
* @Filename: systbcfg.cpp
* @Brief: 
*
**/

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


extern long long _system_TB_config;

int main()
{
	long long ltb1 = _system_TB_config;
	long long ltb2 = 0;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 20000;

	printf("ltb1:%lld \n", ltb1);

	while(1)
	{
		select(NULL, NULL, NULL, NULL, &tv);
		ltb2 = _system_TB_config;
		if(ltb1 != ltb2)
		{
			printf("ltb2:%lld \n", ltb2);;
			ltb1 = ltb2;
		}
	}

	return 0;
}
