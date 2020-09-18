#!/bin/bash
set -x
PROJECTNAME="createJavaVM"
RHOST="ut29p62"
PROJECTSPATH="/QOpenSys/home/zhanggan/worktemp/${PROJECTNAME}"

if [[ "$*" =~ "copycode" ]]; then
    shift
    scp invoke.cpp $RHOST:worktemp/createJavaVM/

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
        JDK=11 make" $RHOST

fi

