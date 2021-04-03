import torch
import torch.nn as nn
import numpy as np
from torch.autograd import Variable
import matplotlib as mlp
mlp.use("Agg")
import matplotlib.pyplot as plt

with open("p46data.txt",'r') as f:
	datalist = f.readlines()
datalist = [i.split('\n')[0] for i in datalist]
datalist = [i.split(',') for i in datalist]
data = [[float(i[0]),float(i[1]),float(i[2])] for i in datalist]
#print(data)



x0 = list(filter(lambda x:x[-1] == 0.0 , data))
x1 = list(filter(lambda x:x[-1] == 1.0 , data))

x0_0 = [i[0] for i in x0]
x0_1 = [i[1] for i in x0]
x1_0 = [i[0] for i in x1]
x1_1 = [i[1] for i in x1]

plt.plot(x0_0,x0_1,"ro",label='x_0')
plt.plot(x1_0,x1_1,"bo",label='x_1')
plt.legend(loc='best')
plt.savefig("p46.init.png")

class LogisticReg(nn.Module):
	def __init__(self):
		super(LogisticReg, self).__init__()
		self.lr= nn.Linear(2,4)
		self.sm = nn.Sigmoid()
	def forward(self,x):
		x = self.lr(x)
		x = self.sm(x)
		return(x)

x_data = [[i[0],i[1]] for i in data]
y_data = [[i[2]] for i in data]
#y_data = [i[2] for i in data]

x_data = torch.tensor(x_data)
y_data = torch.tensor(y_data)
#y_data = y_data.unsqueeze(1)
print(x_data.size(),y_data.size())

logMod = LogisticReg()
#try to initialize params
#logMod.lr.weight[0] = torch.FloatTensor([-1,-1]);
#logMod.lr.bias[0] = torch.FloatTensor([100]);
if torch.cuda.is_available():
	logMod.cuda()
criteria = nn.BCELoss()
optimizer = torch.optim.SGD(logMod.parameters(),lr=1e-3,momentum=0.9)

epoch_count =20000
for epoch in range(epoch_count):
	out = logMod(x_data)
	loss = criteria(out,y_data)
	optimizer.zero_grad()
	loss.backward()
	optimizer.step()
	if(epoch % 1000 == 0):
		print("epoch:{},loss:{}".format(epoch,loss))
logMod.eval()
w0,w1= logMod.lr.weight[0]
w0 = w0.data.item()
w1 = w1.data.item()
b = logMod.lr.bias.data.item()

plot_x = np.arange(30,100,0.1)
plot_y = (-w0*plot_x-b)/w1

#y_out = logMod(x_data)
plt.plot(plot_x,plot_y,"g")
plt.savefig("p46.logistic.png")
