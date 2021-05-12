import torch
from torch.autograd import Variable
import matplotlib as mlp
mlp.use("Agg")
import matplotlib.pyplot as plt 
import numpy as np

x_data_l = [-5.0,-3.5,-1,-0.8,-0.5,0,0.2,0.23,0.28,0.32,0.35,0.78,0.82,0.87,0.92,0.94,1.0,1.2,1.23,1.25,1.4,2,2.5,3,3.2,4,4.8]
y_data_l = [1,1,1,1,1,1,0,1,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,0,0]

x_data = torch.tensor(x_data_l)
y_data = torch.FloatTensor(y_data_l)
x_input = x_data.unsqueeze(1)
y_output = y_data.unsqueeze(1)


w  = Variable(torch.randn(1),requires_grad=True)
b  = Variable(torch.zeros(1),requires_grad=True)
def l_lr(x):
	return(w*x+b)
def l_sigmod(x):
	return(1/(1+torch.exp(x)))
epoch_count = 10000
class MyModel(torch.nn.Module):
	def __init__(self):
		super(MyModel,self).__init__()
		self.lr=l_lr;
		self.sig = l_sigmod;
	def forward(self,x):
		out = self.lr(x)
		out = self.sig(out)
		return out
	def parameters(self):
		return (w,b)
model = MyModel();
#model = torch.nn.Linear(1,1)
#crit = torch.nn.L1Loss(reduction='elementwise_mean')
crit = torch.nn.BCELoss(reduction='elementwise_mean')
optimizer = torch.optim.SGD(model.parameters(),lr=1e-2)

for epoch in range(epoch_count):
	out = model(x_input)
	loss = crit(out,y_output)
	optimizer.zero_grad()
	loss.backward()
	optimizer.step()
	if(epoch%1000==0):
		print("epoch:{},loss:{}".format(epoch,loss))
plt.plot(x_data_l,y_data_l,"b+")

print("w={},b={},x={}".format(w.data.item(),b.data.item(),-b.data.item()/w.data.item()))
s = np.linspace(-1,5,60)
t = s*w.data.item() +b.data.item()

plt.plot(s,t,"g")
model.eval()
y_curv = model(x_input)
plt.plot(x_data_l,y_curv.squeeze(1).data.numpy(),"r")
plt.xticks(np.arange(-5,5,0.2),rotation=90)
plt.yticks(np.arange(-2,5,0.2))
plt.grid(True)
plt.savefig("p45.logistic.png")
torch.save(model,"p45.m.pht")
