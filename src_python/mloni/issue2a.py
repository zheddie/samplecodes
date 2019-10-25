import torch
x = torch.zeros(2,4).float()
print("x:",x)
y= torch.randn(2,4,dtype=torch.float32).float()
print("y:",y)
# z = x*y
# print("z:x*y:",z)
z2 = x+y
z2=z2.float()
print("z2:x+y:",z2)