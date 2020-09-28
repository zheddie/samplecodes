#! /usr/local/bin/bash
set -x
PROJECTNAME="samplecodes"
#RHOST="ut28p65"
RHOST="ut20p31"   #i75

GITHUBREPO="git@github.com:zheddie/samplecodes.git"
SUBPROJ="ILE"

if [ "$1"  == "init" ]; then
	rcmd "cd && git clone $GITHUBREPO" $RHOST

fi

if [ "$1"  == "ILE.copycode" ]; then
	scp ~/samplecodes/src_C/ILE/makefile $RHOST:samplecodes/src_C/ILE/
	# scp ~/samplecodes/src_C/ILE/mymodule4.c $RHOST:samplecodes/src_C/ILE/
	#scp ~/samplecodes/src_C/ILE/PASEtoILE4.c $RHOST:samplecodes/src_C/ILE/
	
fi

if [ "$1"  == "ILE.makesrvpgm" ]; then
	TARGET="srvpgm"
	rcmd "cd /qopensys/home/zhanggan/samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.makepasepgm" ]; then
	TARGET="pasepgm"
	rcmd "cd /qopensys/home/zhanggan/samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.makeall" ]; then
	TARGET="all"
	rcmd "cd /qopensys/home/zhanggan/samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.makerun" ]; then
	TARGET="run"
	rcmd "cd /qopensys/home/zhanggan/samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.run" ]; then
	rcmd "cd /qopensys/home/zhanggan/samplecodes/src_C/ILE/ && ./pase2ile " $RHOST
fi