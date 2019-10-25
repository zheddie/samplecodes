#!/usr/bin/bash
#export BIPYTHON="g++ -pthread -bI:/QOpenSys/pkgs/lib/python3.6/config-3.6m/python.exp"
#export LDSOAIX="/QOpenSys/pkgs/lib/python3.6/config-3.6m/ld_so_aix"
#export LDSHARED="$LDSOAIX $BIPYTHON"
export LDSHARED="/QOpenSys/pkgs/lib/python3.6/config-3.6m/ld_so_aix g++ -pthread -bI:/QOpenSys/pkgs/lib/python3.6/config-3.6m/python.exp"
export CXX=g++ NO_CUDA=1 NO_CUDNN=1 NO_TEST=1 NO_DISTRIBUTED=1 
echo $LDSHARED $CXX $NO_CUDA $NO_CUDNN
python3 setup.py build
