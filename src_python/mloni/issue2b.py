import torch

# xh = torch.FloatTensor([[2.0,3.,4.,0.]])
# print("xh:",xh)
# yh = torch.FloatTensor([[1.0,0.,0.,0.]])
# print("yh:",yh)
# zh = xh+yh
# print("Half:zh(xh+yh):",zh)



xf = torch.FloatTensor([[2.0,3.,4.,0.]])
print("xf:",xf)
yf = torch.FloatTensor([[1.0,0.,2.,0.]])
print("yf:",yf)
zf = xf+yf
zf2 = torch.add(xf,0.5)
print("Float:zf(xf+yf):",zf)
print("Float:zf2(xf+0.5):",zf2)

yf2=xf*2
print("yf2(xf*2):",yf2)


# xd = torch.DoubleTensor([[2.,3.,4.,0.]])
# print("xd:",xd)
# yd = torch.DoubleTensor([[1.,0.,0.,0.]])
# print("yd:",yd)
# zd = xd+yd
# print("Double:zd(xd+yd):",zd)


# xb = torch.IntTensor([[2,3,4,0]])
# print("xb:",xb)
# yb = torch.IntTensor([[1,0,0,0]])
# print("yb:",yb)
# zb = xb+yb
# print("Byte:zb(xb+yb):",zb)

# xc = torch.IntTensor([[2,3,4,0]])
# print("xc:",xc)
# yc = torch.IntTensor([[1,0,0,0]])
# print("yc:",yc)
# zc = xc+yc
# print("Char:zc(xc+yc):",zc)

# xs = torch.ShortTensor([[2,3,4,0]])
# print("xs:",xs)
# ys = torch.ShortTensor([[1,0,0,0]])
# print("ys:",ys)
# zs = xs+ys
# print("Short:zs(xs+ys):",zs)

# xi = torch.IntTensor([[2,3,4,0]])
# print("xi:",xi)
# yi = torch.IntTensor([[1,0,0,0]])
# print("yi:",yi)
# zi = xi+yi
# print("Int:zi(xi+yi):",zi)


# xl = torch.IntTensor([[2,3,4,0]])
# print("xl:",xl)
# yl = torch.IntTensor([[1,0,0,0]])
# print("yl:",yl)
# zl = xl+yl
# print("Long:zl(xl+yl):",zl)


