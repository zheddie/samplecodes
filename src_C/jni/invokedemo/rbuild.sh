#!/bin/bash
set -x
PROJECTNAME="invokedemo"
#RHOST="ut29p62"
RHOST=$1
shift
PROJECTSPATH="/QOpenSys/home/zhanggan/worktemp/${PROJECTNAME}"

if [[ "$*" =~ "copycode" ]]; then
    shift
    rcmd "mkdir -p ${PROJECTSPATH}" $RHOST
    if [[ "$*" == "" ]]; then
        scp * $RHOST:${PROJECTSPATH}/
    else
        scp $* $RHOST:${PROJECTSPATH}/
    fi
fi
if [[ "$*" =~ "make" ]]; then
    shift
    rcmd " cd ${PROJECTSPATH} && \
        make $*" $RHOST
fi
