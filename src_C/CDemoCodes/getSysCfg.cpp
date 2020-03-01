/*
 * getSysCfg.cpp
 *
 *  Created on: Jun 3, 2014
 *      Author: zhanggan
 */
#include <iostream>
#include "getSysCfg.h"
#include <sys/systemcfg.h>
#if !defined(SC_TM_VER)
#define SC_TM_VER 59
#endif  /* !defined(SC_TM_VER) */

int main(int argc, char * argv[]){
	getSysCfg * a= new getSysCfg();
	a->print();
	a->getsystemcfg(SC_TM_VER);
	return (0);
}
getSysCfg::getSysCfg() {
	// TODO Auto-generated constructor stub

}

getSysCfg::~getSysCfg() {
	// TODO Auto-generated destructor stub
}

void getSysCfg::print(){
	std::cout<<_system_configuration.phys_implementation<<std::endl;
}
int getSysCfg::getsystemcfg(int cfg){
	long lt = (long)getsystemcfg(SC_TM_VER);
	printf("Systemcfg(%d)=%l\n",cfg,lt);
	//std::cout<<"Systemcfg("<<cfg<<")="<<(long)getsystemcfg(cfg)<<std::endl;
	return(0);
}
