import torch


xf = torch.FloatTensor([[2.0,3.,4.,0.]])
print("xf:",xf)
yf = torch.FloatTensor([[1.0,0.,0.,0.]])
print("yf:",yf)
zf = xf+yf
print("Float:zf(xf+yf):",zf)

yf2=xf*2
print("yf2(xf*2):",yf2)


import numpy as np 
nx = np.array([2.0,3.,4.,0.],dtype="float32")
ny = np.array([1.0,0.,0.,0.],dtype="float32")
print("nx:",nx)
print("ny:",ny)
nz = nx+ny
print("float32:nz(nx+ny):",nz)

nz2=nx*2
print("nz2(nx*2):",nz2)



