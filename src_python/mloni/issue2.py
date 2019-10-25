import torch
import torch.nn as nn

lstm = nn.LSTM(3,4)
x = torch.randn(1,2,3,dtype=torch.float64)
y ,(h,o)= lstm(x)
print("x=",x.size(),x)
print("y=",y.size(),y)
print("h=",h.size(),h)
print("o=",o.size(),o)
