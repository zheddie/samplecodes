import torch
from torch.autograd import *
b  = torch.Tensor([4,5,6,7])
a = Variable(torch.Tensor([1,2,3]))
print (a,a.size())
a.resize_(3,1)
print (a,a.size())
print(a.size()[0])
