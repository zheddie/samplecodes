#!/bin/bash
set -x
PROJECTNAME="createJavaVM"
#RHOST="ut29p62"
RHOST=$1
shift
PROJECTSPATH="/QOpenSys/home/zhanggan/worktemp/${PROJECTNAME}"

if [[ "$*" =~ "copycode" ]]; then
    shift
    scp $* $RHOST:worktemp/createJavaVM/
#    scp invoke.cpp $RHOST:worktemp/createJavaVM/
#    scp pase_invoke.c $RHOST:worktemp/createJavaVM/
#    scp Makefile $RHOST:worktemp/createJavaVM/
#    scp Prog.java $RHOST:worktemp/createJavaVM/

fi
if [[ "$*" =~ "make11" ]]; then
    shift
    rcmd " cd ${PROJECTSPATH} && \
        make clean && \
        JDK=11 make" $RHOST

fi
if [[ "$*" =~ "make80" ]]; then
    shift
    rcmd " cd ${PROJECTSPATH} && \
        make clean && \
        JDK=80 make" $RHOST
fi

