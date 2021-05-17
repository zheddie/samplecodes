import torch
import torch.nn as nn
import numpy as np
import matplotlib as mpl
mpl.use("Agg")
import matplotlib.pyplot as plt

modelfile = "p38.model.pht"
modelparmfile= "p38.parms.pht"
batchsize = 55
epoch_count = 3000
w = torch.tensor([0.5,3,2.4])
b = torch.tensor([0.9])

def transform(x):
	x  = x.unsqueeze(1);
	rt = torch.cat([x**i for i in range(1,4) ],1)
	return(rt);
def f(x):
	y = torch.mm(x,w.unsqueeze(1))+b
	return(y)
if __name__ == '__main__':
	xn = torch.randn(batchsize);
	#print(xn.size())
	x,_ = torch.sort(xn)# torch.tensor(np.array(torch.sort(xn)))
	print(x)
	xt = transform(x)
	y = f(xt)

	# print(x.size(),inputs.size())
	# print(x,inputs)
	#Try to linear regression on it.
	lrm = nn.Linear(3,1)
	crit = nn.MSELoss()
	optimizer = torch.optim.SGD(lrm.parameters(),lr=1e-2)
	for epoch in range(epoch_count):
		out = lrm(xt)
		loss = crit(out,y)
		optimizer.zero_grad()
		loss.backward()
		optimizer.step()
		if(epoch % 100 == 0):
			print("epoch:{},loss={}".format(epoch,loss.data.item()))
	lrm.eval()
	yn = lrm(xt)
	plt.plot(x.numpy(),yn.detach().squeeze(1).numpy(),"r")
	plt.plot(x.numpy(),y.squeeze(1).numpy(),"bo")
	plt.savefig("p38data.png")
	print(w,b,lrm.state_dict())
	torch.save(lrm,modelfile)
	torch.save(lrm.state_dict(),modelparmfile)
	