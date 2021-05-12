import matplotlib as mlp
import numpy as np
mlp.use("Agg")
import matplotlib.pyplot as plt 

x_data_l = [-5.0,-3.5,-1,-0.8,-0.5,0,0.2,0.23,0.28,0.32,0.35,0.78,0.82,0.87,0.92,0.94,1.0,1.2,1.23,1.25,1.4,2,2.5,3,3.2,4,4.8]
y_data_l = [1,1,1,1,1,1,0,1,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,1,0,0]

z = 0

def l_func(x):
	out = []
	i = 0
	for one in x:
		if one >= z:
			out.append(0)
		else:
			out.append(1)
		i+=1
	#print("zg.out=",out)
	return(out)

def f_loss(y_,y):
	ls = 0
	i = 0
	for one in y:
		if one != y_[i]:
			ls+=1
		i+=1
	#print("zg.ls=",ls)
	return ls

epoch_count = 2000
class MyModel():
	def __init__(self):
		self.myfunc=l_func;
	def forward(self,x):
		out = self.myfunc(x)
		return out
	def __call__(self,x):
		return self.forward(x)
model = MyModel();

crit=f_loss
#optimizer = torch.optim.SGD(model.parameters(),lr=1e-3)
minloss= 65535
minz = 0
outz = 0
for epoch in range(epoch_count):
	out = model(x_data_l)
	#print(out,y_data_l)
	loss = crit(out,y_data_l)
	if(loss <=minloss):
		minloss = loss
		outz = z
	z += 1e-3 
	if(epoch%100==0):
		print("epoch:{},loss:{}".format(epoch,loss))
z = outz
print("zg.outz=",z)
y_line = model(x_data_l)

plt.plot(x_data_l,y_data_l,"b+")

plt.plot(x_data_l,y_line,"r")
plt.xticks(np.arange(-5,5,0.2),rotation=90)
plt.yticks(np.arange(-0.2,1.2,0.1))
plt.grid(True)
plt.savefig("p45.linear.png")

