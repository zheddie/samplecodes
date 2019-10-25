from distutils.core import setup, Extension
setup(name='helloworld', version='1.0',  \
      ext_modules=[Extension('helloworld', ['hello.cpp'],extra_compile_args = ["-std=c++11","-fPIC"])])
