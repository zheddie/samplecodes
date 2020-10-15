#! /usr/local/bin/bash
set -x
PROJECTNAME="samplecodes"
RHOSTHOME="/qopensys/home/zhanggan"
#RHOST="ut20p31"   #i75

GITHUBREPO="git@github.com:zheddie/samplecodes.git"
SUBPROJ="ILE"

if [ "$1"  == "init" ]; then
	rcmd "cd && git clone $GITHUBREPO" $RHOST

fi
##################setup env#########################
if [[ "$*" =~ "sig." ]]; then
	RHOST="ut28p65"
	SRCPATH="samplecodes/src_java/jnisignal/"
fi
if [[ "$*" =~ "ILE." ]]; then
	RHOST="ut20p31"   #i75
	SRCPATH="samplecodes/src_C/ILE/"
fi

###################signal/CPS issue###################################


if [[ "$*" =~ "sig.copycode" ]]; then
	# scp ~/samplecodes/src_java/jnisignal/makefile $RHOST:$SRCPATH
	# scp /Volumes/CSDISK/mycode/samplecodes/src_java/jnisignal/xlcmake.mk $RHOST:$SRCPATH
	scp /Volumes/CSDISK/mycode/samplecodes/src_java/jnisignal/PaseExample1.c $RHOST:$SRCPATH
	scp /Volumes/CSDISK/mycode/samplecodes/src_java/jnisignal/PaseExample1.java $RHOST:$SRCPATH
fi

if [ "$1"  == "sig.gccmake" ]; then
	shift
	rcmd "cd ${RHOSTHOME}/$SRCPATH && make $*" $RHOST
fi
if [ "$1"  == "sig.xlcmake" ]; then
	shift
	rcmd "cd ${RHOSTHOME}/$SRCPATH && make -f xlcmake.mk $*" $RHOST
fi
#######################ILE/SAP issue###############################
if [ "$1"  == "ILE.copycode" ]; then
	scp ~/samplecodes/src_C/ILE/makefile $RHOST:samplecodes/src_C/ILE/
	# scp ~/samplecodes/src_C/ILE/mymodule4.c $RHOST:samplecodes/src_C/ILE/
	#scp ~/samplecodes/src_C/ILE/PASEtoILE4.c $RHOST:samplecodes/src_C/ILE/
fi

if [ "$1"  == "ILE.makesrvpgm" ]; then
	TARGET="srvpgm"
	rcmd "cd ${RHOSTHOME}samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.makepasepgm" ]; then
	TARGET="pasepgm"
	rcmd "cd ${RHOSTHOME}samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.makeall" ]; then
	TARGET="all"
	rcmd "cd ${RHOSTHOME}samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.makerun" ]; then
	TARGET="run"
	rcmd "cd ${RHOSTHOME}samplecodes/src_C/ILE/ && make ${TARGET}" $RHOST
fi
if [ "$1"  == "ILE.run" ]; then
	rcmd "cd ${RHOSTHOME}samplecodes/src_C/ILE/ && ./pase2ile " $RHOST
fi