#!/bin/bash
PYTORCHHOME="/Users/zhanggan/mycode/ibmiprojects/"
cd ${PYTORCHHOME}
curl -o pytorch-1.0.0.tar.gz https://codeload.github.com/pytorch/pytorch/tar.gz/v1.0.0
tar zxf pytorch-1.0.0.tar.gz
mv pytorch-1.0.0 pytorch
cd ${PYTORCHHOME}/pytorch/third_party
git clone https://github.com/facebookincubator/gloo
git clone https://github.com/pybind/pybind11.git
git clone https://github.com/Maratyszcza/cpuinfo.git
git clone https://github.com/onnx/onnx.git
git clone https://github.com/pytorch/QNNPACK
git clone https://github.com/pytorch/fbgemm
#for build failure
git clone https://github.com/google/googletest.git
git clone https://github.com/google/benchmark.git
git clone https://github.com/Maratyszcza/NNPACK.git
git clone https://github.com/Maratyszcza/pthreadpool.git
git clone https://github.com/Maratyszcza/FXdiv.git
git clone https://github.com/Maratyszcza/FP16.git
git clone https://github.com/Maratyszcza/psimd.git
git clone https://github.com/google/protobuf.git
git clone https://github.com/shibatch/sleef
cd ${PYTORCHHOME}
tar zcf pytorch.src.beforebuild.tar.gz pytorch
cd ${PYTORCHHOME}/pytorch
#python3 setup.py build
