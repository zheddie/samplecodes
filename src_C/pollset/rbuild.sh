#!/bin/bash
set -x
OSNAME=$1
PROJECTNAME=""
if [[ "$OSNAME" == "400" ]]; then
	HOST="ut28p65"
fi
if [[ "$OSNAME" == "aix" ]]; then
	#HOST="aixvnc01"
	HOST="ut31p32"
fi
shift
if [[ "$*" =~ "copycode" ]]; then
	scp cpollset.c Makefile socketClient.c socketServer.c socket.srv.pollset.c $HOST:pollset/ 
fi
if [[ "$*" =~ "makeall" ]]; then
	ssh $HOST "cd pollset &&PATH=/usr/bin:/QOpenSys/usr/bin:/QOpenSys/pkgs/bin:/QOpenSys/xlcpp/opt/IBM/xlC/13.1.3/bin/ make all"
fi
if [[ "$*" =~ "healthcheck" ]]; then
	ssh $HOST "cd pollset &&dump -TvX64 libpollset.so &&/QOpenSys/home/zhanggan/bin/ldd sspsps"
fi