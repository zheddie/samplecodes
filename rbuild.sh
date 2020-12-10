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
	#RHOST="ut28p65"
	RHOST="ut30p12"
	SRCPATH="samplecodes/src_java/jnisignal/"
fi
if [[ "$*" =~ "ILE." ]]; then
	RHOST="ut20p31"   #i75
	SRCPATH="samplecodes/src_C/ILE/"
fi

if [[ "$*" =~ "smp." ]]; then
	RHOST="ut28p65"
	SRCPATH="samplecodes/src_java/ilecall/"
fi
if [[ "$*" =~ "pb." ]]; then
	RHOST="ut30p12"
	SRCPATH="samplecodes/src_java/processbuilder/"
fi
if [[ "$*" =~ "cli." ]]; then
	RHOST="ut28p64"
	SRCPATH="samplecodes/src_C/cli/"
fi
scopy(){
	filename=$1
	scp $filename $RHOST:$SRCPATH
}
##################processbuilder sample ############
if [ "$1"  == "pb.copycode" ]; then
	scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/processbuilder/PBCall.java"
	rcmd "cd ${RHOSTHOME}/$SRCPATH && javac PBCall.java" $RHOST
fi
if [ "$1"  == "pb.build" ]; then
	rcmd "cd ${RHOSTHOME}/$SRCPATH && javac PBCall.java" $RHOST
fi
if [ "$1"  == "pb.run" ]; then
	#rcmd "cd ${RHOSTHOME}/$SRCPATH && java PBCall PBTest" $RHOST
	rcmd "cd ${RHOSTHOME}/$SRCPATH && java PBCall zgSleep 60" $RHOST
fi
##################sample of ILE call################
if [ "$1"  == "smp.copycode" ]; then
	scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/ilecall/callILENative.java" 
	scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/ilecall/makefile" 
	scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/ilecall/callILENative.c" 
fi
if [ "$1"  == "smp.make" ]; then
	shift
	rcmd "cd ${RHOSTHOME}/$SRCPATH && make $*" $RHOST
fi
###################signal/CPS issue###################################


if [[ "$*" =~ "sig.copycode" ]]; then
	#scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/jnisignal/makefile"
	scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/jnisignal/xlcmake.mk"
	scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/jnisignal/PaseExample1.c"
	scopy "/Volumes/CSDISK/mycode/samplecodes/src_java/jnisignal/PaseExample1.java"
fi

if [ "$1"  == "sig.gccmake" ]; then
	shift
	rcmd "cd ${RHOSTHOME}/$SRCPATH && make $*" $RHOST
fi
#sig.xlcmake ilecomile
#sig.xlcmake run?
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
if [ "$1"  == "cli.copycode" ]; then
	scp /Volumes/CSDISK/mycode/samplecodes/src_C/cli/embedded.c $RHOST:$SRCPATH
fi

if [ "$1"  == "cli.build" ]; then
	rcmd "cd ${RHOSTHOME}/$SRCPATH && xlC -o clitest embedded.c" $RHOST
fi