from distutils.core import setup, Extension
import os
os.environ["CC"]="g++"
module1 = Extension('demo',
                    sources = ['demo.cpp']
                    #,language="c++"
                    #,extra_compile_args = ["-Wl,-brtl -L/QOpenSys/pkgs/lib -maix64"]
                    )

setup (name = 'PackageName',
       version = '1.0',
       description = 'This is a demo package',
       ext_modules = [module1])
