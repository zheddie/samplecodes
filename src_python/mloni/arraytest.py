import torch
import numpy as np
from torch.autograd import Variable


x = torch.from_numpy(np.array([[ 1.],[ 2.],[ 3.],[ 4.],[ 5.],[ 6.],[ 7.],[ 8.],[ 9.],[10.],[11.],[12.],[13.],[14.],[15.],[16.],[17.],[18.],[19.],[20.],[21.],[22.],[23.],[24.],[25.],[26.],[27.],[28.],[29.],[30.],[31.],[32.],[33.],[34.],[35.],[36.],[37.],[38.],[39.],[40.],[41.],[42.],[43.],[44.],[45.],[46.],[47.],[48.],[49.],[50.],[51.],[52.],[53.],[54.],[55.],[56.],[57.],[58.],[59.],[60.],[61.],[62.],[63.],[64.],[65.],[66.]]))
w= Variable(torch.from_numpy(np.array([0.0018])),requires_grad=True)
b= Variable(torch.from_numpy(np.array([0.4471])),requires_grad=True)

yy = x*w+b

print("yy:",yy)