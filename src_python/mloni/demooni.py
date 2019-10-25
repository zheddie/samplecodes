import matplotlib as mlp
mlp.use("Agg")
import matplotlib.pyplot as plt
import numpy as np
data = [10.94, 0.2, 0.23, 0.21, 0.23, 0.21, 0.22, 0.21, 0.21, 0.21, 0.22, 0.21, 0.22, 0.22, 0.23, 0.22, 0.21, 0.21, 0.22, 0.24, 0.21, 0.33, 1.17, 0.23, 10.11, 0.22, 0.23, 0.2, 0.24, 0.21, 0.24, 0.22, 0.21, 0.22, 0.22, 0.21, 0.22, 0.23, 0.22, 0.21, 0.21, 0.22, 0.21, 0.23, 0.21, 0.23, 1.08, 0.22, 10.36, 0.21, 0.24, 0.2, 0.25, 0.22, 0.31, 0.18, 0.19, 0.2, 0.21, 0.2, 0.2, 0.19, 0.2, 0.18, 0.18, 0.2, 0.19, 0.24, 0.19, 0.2, 1.07, 0.2, 10.66, 0.19, 0.2, 0.2, 0.19, 0.2, 0.21, 0.19, 0.2, 0.37, 0.22, 0.18, 0.31, 0.19, 0.2, 0.19, 0.23, 0.19, 0.22, 0.19, 0.19, 0.19, 1.05, 0.19, 11.01, 0.19, 0.19, 0.2, 0.19, 0.19, 0.2, 0.19, 0.2, 0.25, 0.2, 0.2, 0.27, 0.18, 0.21, 0.19, 0.2, 0.19, 0.2, 0.19, 0.2, 0.2, 1.07, 0.18, 10.5, 0.19, 0.19, 0.19, 0.19, 0.18, 0.21, 0.21, 0.2, 0.18, 0.19, 0.21, 0.19, 0.2, 0.19, 0.19, 0.21, 0.18, 0.22, 0.19, 0.21, 0.22, 1.29, 0.25, 10.18, 0.22, 0.21, 0.22, 0.22, 0.2, 0.25, 0.22, 0.22, 0.21, 0.23, 0.22, 0.2, 0.24, 0.22, 0.32, 0.22, 0.21, 0.23, 0.24, 0.24, 0.23, 1.41, 0.23, 11.95, 0.2, 0.24, 0.21, 0.23, 0.2, 0.22, 0.23, 0.21, 0.24, 0.21, 0.22, 0.21, 0.21, 0.24, 0.24, 0.23, 0.24, 0.24, 0.26, 0.24, 0.24, 1.05, 0.32, 11.22, 0.21, 0.24, 0.21, 0.22, 0.23, 0.97, 0.27, 0.3, 0.22, 0.22, 0.22, 0.21, 0.21, 0.22, 0.22, 0.21, 0.23, 0.22, 0.23, 0.21, 0.21, 1.08, 0.23, 10.67, 0.21, 0.21, 0.21, 0.21, 0.21, 0.22, 0.28, 0.22, 0.27, 0.24, 0.22, 0.2, 0.22, 0.23, 0.23, 0.26, 0.22, 0.22, 0.22, 0.22, 0.2, 1.04, 0.22, 9.78, 0.2, 0.2, 0.19, 0.22, 0.21, 0.22, 0.2, 0.2, 0.22, 0.21, 0.24, 0.22, 0.21, 0.22, 0.27, 0.22, 0.21, 0.23, 0.22, 0.22, 0.22, 1.06, 0.22, 10.45, 0.2, 0.22, 0.21, 0.21, 0.21, 0.22, 0.2, 0.21, 0.2, 0.22, 0.19, 0.21, 0.28, 0.22, 0.2, 0.22, 0.21, 0.23, 0.22, 0.23, 0.21, 1.03, 0.25, 10.38, 0.21, 0.24, 0.28, 0.23, 0.23, 0.26, 0.22, 0.21, 0.21, 0.22, 0.21, 0.21, 0.22, 0.21, 0.23, 0.24, 0.21, 0.23, 0.21, 0.24, 0.22, 1.02, 0.24, 10.86, 0.23, 0.24, 0.21, 0.24, 0.23, 0.22, 0.21, 0.23, 0.22, 0.22, 0.22, 0.23, 0.23, 0.22, 0.22, 0.2, 0.24, 0.24, 0.21, 0.23, 0.22, 1.0, 0.23, 10.28, 0.2, 0.22, 0.22, 0.25, 0.21, 0.22, 0.24, 0.68, 0.22, 0.22, 0.22, 0.21, 0.21, 0.22, 0.22, 0.21, 0.22, 0.24, 0.21, 0.22, 0.21, 1.78, 0.25, 10.6, 0.2, 0.28, 0.24, 0.2, 0.2, 0.27, 0.24, 0.23, 0.21, 0.2, 0.21, 0.21, 0.19, 0.2, 0.21, 0.2, 0.22, 0.2, 0.21, 0.2, 0.15, 1.04, 0.27, 10.59, 0.2, 0.21, 0.2, 0.19, 0.18, 0.23, 0.22, 0.22, 0.24, 0.2, 0.22, 0.21, 0.2, 0.21, 0.18, 0.2, 0.2, 0.21, 0.21, 0.19, 0.21, 0.98, 0.2, 10.32, 0.19, 0.2, 0.2, 0.2, 0.19, 0.21, 0.18, 0.21, 0.19, 0.2, 0.2, 0.2, 0.2, 0.23, 0.19, 0.2, 0.23, 0.2, 0.22, 0.2, 0.23, 0.96, 0.2, 9.37, 0.19, 0.22, 0.2, 0.2, 0.24, 0.2, 0.21, 0.2, 0.21, 0.21, 0.19, 0.2, 0.22, 0.21, 0.19, 0.21, 0.18, 0.22, 0.22, 0.2, 0.2, 0.94, 0.19, 10.18, 0.21, 0.2, 0.21, 0.22, 0.41, 0.41, 0.39, 0.38, 0.4, 0.41, 0.41, 0.4, 0.21, 0.2, 0.2, 0.21, 0.18, 0.22, 0.19, 0.2, 0.19, 0.93, 0.2, 9.5, 0.18, 0.21, 0.19, 0.21, 0.21, 0.23, 0.21, 0.19, 0.23, 0.22, 0.2, 0.21, 0.2, 0.23, 0.21, 0.2, 0.19, 0.33, 0.2, 0.2, 0.2, 0.98, 0.21, 10.58, 0.19, 0.21, 0.2, 0.19, 0.21, 0.21, 0.2, 0.18, 0.2, 0.22, 0.2, 0.21, 0.2, 0.21, 0.21, 0.21, 0.25, 0.21, 0.2, 0.21, 0.2, 0.92, 0.21,10.16, 0.22, 0.2, 0.2, 0.2, 0.19, 0.2, 0.21, 0.21, 0.2, 0.21, 0.21, 0.19, 0.21, 0.21, 0.2, 0.19, 0.2, 0.22, 0.2, 0.2, 0.19, 0.86, 0.22, 10.24, 0.2, 0.22, 0.14, 0.26, 0.25, 0.26, 0.24, 0.24, 0.27, 0.24, 0.47, 0.24, 0.23, 0.25, 0.24, 0.19, 0.22, 0.22, 0.21, 0.21, 0.22, 0.88, 0.21, 10.79, 0.2, 0.21, 0.21, 0.2, 0.21, 0.22, 0.21, 0.2, 0.2, 0.21, 0.2, 0.2, 0.2, 0.21, 0.22, 0.21, 0.24, 0.21, 0.22, 0.2, 0.2, 0.86, 0.22, 10.34, 0.21, 0.75, 0.21, 0.18, 0.16, 0.19, 0.17, 0.16, 0.23, 0.16, 0.16, 0.16, 0.16, 0.18, 0.17, 0.16, 0.16, 0.17, 0.17, 0.16, 0.16, 0.8, 0.16, 10.53, 0.18, 0.17, 0.16, 0.16, 0.16, 0.17, 0.16, 0.16, 0.16, 0.17, 0.17, 0.15, 0.15, 0.17, 0.16, 0.17, 0.15, 0.16, 0.15, 0.17, 0.16, 0.79, 0.15, 11.73, 0.16, 0.16, 0.18, 0.16, 0.16, 0.17, 0.16, 0.19, 0.18, 0.17, 0.16, 0.17, 0.15, 0.17, 0.16, 0.16, 0.15, 0.17, 0.16, 0.18, 0.16, 0.82, 0.16, 10.98, 0.79, 0.18, 0.2, 0.2, 0.14, 0.14, 0.15, 0.2, 0.2, 0.21, 0.2, 0.2, 0.21, 0.31, 0.19, 0.21, 0.2, 0.22, 0.19, 0.22, 0.2, 0.87, 0.2, 10.4, 0.18, 1.49, 0.2, 0.2, 0.2, 0.21, 0.19, 0.19, 0.21, 0.21, 0.22, 0.2, 0.2, 0.21, 0.21, 0.19, 0.2, 0.19, 0.22, 0.19, 0.21, 0.94, 0.2, 12.29, 0.22, 0.21, 0.2, 0.2, 0.19, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.21, 0.21, 0.19, 0.2, 0.2, 0.45, 0.21, 0.2, 0.21, 0.92, 0.22, 9.77, 0.19, 0.25, 0.2, 0.2, 0.19, 0.22, 0.2, 0.21, 0.23, 0.21, 0.19, 0.22, 0.2, 0.2, 0.2, 0.2, 0.2, 0.34, 0.19, 0.21, 0.21, 1.07, 0.18, 10.7, 0.19, 0.24, 0.19, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.21, 0.18, 0.19, 0.21, 0.22, 0.19, 0.21, 0.21, 0.21, 0.19, 0.2, 0.19, 0.91, 0.2]

#for x in [1,6,8,12,24,24*7]:
for x in [12,24]:
    datlen = len(data)
    idx = 0
    X_train = []
    while idx<datlen-x:
        eachlist = data[idx:idx+x]
        eachmean = sum(eachlist)/len(eachlist)
        X_train.append(eachmean)
        idx+=x
    eachlist = data[idx:]
    eachmean = sum(eachlist)/len(eachlist)
    X_train.append(eachmean)
    if x== 24:
        onedaydata = X_train
    if x== 12:
        halfdaydata = X_train
    # xlen = len(X_train)
    # plt.plot(range(xlen),X_train,"b")
    # plt.show()
datatouse = halfdaydata
ydata1d = np.asarray(datatouse,dtype=np.float32)
ydata =  np.reshape(ydata1d,(-1,1))
xdata1d = np.arange(1,len(datatouse)+1,dtype=np.float32)
xdata =  np.reshape(xdata1d,(-1,1))

xx = np.polyfit(xdata1d, ydata1d, 1)
#print(xx)
targethours = -xx[1]/xx[0]
print(targethours)


from sklearn.linear_model import LinearRegression

model = LinearRegression()
model.fit(xdata,ydata)
pred = model.predict(xdata)
#params = model.get_params()
#print("intercept_:",model.intercept_)
#print('Coefficients: \n', model.coef_)
print("expect time:",(-model.intercept_/model.coef_)[0][0])

plt.plot(xdata,ydata,"b.")
plt.plot(xdata,pred,'r',linewidth=4)
plt.savefig("demoscikit.png")


import scipy as sp

(w,b) = sp.polyfit(xdata1d,ydata1d,1)
print("expect time:",-b/w)
plt.plot(xdata1d,ydata1d,"b.")
plt.plot(xdata1d,xdata1d*w+b,'r',linewidth=4)
plt.savefig("demoscipypolyfit.png")



import scipy as sp

fp1,residuals,rank,sv,rcond = sp.polyfit(xdata1d,ydata1d,1,full=True)

f1 = sp.poly1d(fp1)
fx = sp.linspace(0,xdata1d[-1],10000)
plt.plot(xdata1d,ydata1d,"b.")
plt.plot(fx,f1(fx),'r',linewidth=4)
plt.savefig("demoscipypoly1d.png")

import torch 
import numpy as np
from torch.autograd import Variable
from torch import *


#print(xdata[:20])
class zLinearRegression(torch.nn.Module):
    def __init__(self):
        super(zLinearRegression,self).__init__()
        self.linear = torch.nn.Linear(1,1)
    def forward(self,x):
        out = self.linear(x)
        return out
model = zLinearRegression()
criterion = torch.nn.MSELoss()
optimizer = torch.optim.SGD(model.parameters(),lr=1e-5)
num_epochs = 100000
inputs = Variable(torch.from_numpy(xdata))
target = Variable(torch.from_numpy(ydata))
for epoch in range(num_epochs):

    out = model(inputs)

    loss = criterion(out,target)
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()
    if epoch > 6000 and epoch % 10000 == 0 :
        y_p = out.data.numpy()
        plt.plot(xdata,y_p,'r',linewidth=4)    
        print('epoch: {}, loss: {}'.format(epoch, loss.data.item()))
plt.plot(xdata,y_p,'g',linewidth=4)   
print("weight:",model.linear.weight.data[0],"bias:",model.linear.bias.data.item())
expect = -model.linear.bias.data[0]/model.linear.weight.data.item()
print("expect target:",expect)
plt.plot(xdata,ydata,"b.",label='Original data') 
plt.savefig("demopytorchlinear.png")

import torch 
import numpy as np
from torch.autograd import Variable


y_train = Variable(torch.from_numpy(ydata))
x_train = Variable(torch.from_numpy(xdata))

#print(x_train,y_train)
# w = Variable(torch.randn(1),requires_grad=True)
# b = Variable(torch.zeros(1),requires_grad=True)
w = Variable(torch.randn(1), requires_grad=True)#.type(torch.FloatTensor) # 随机初始化
#b = Variable(torch.zeros(1), requires_grad=True)#.type(torch.FloatTensor) # 使用 0 进行初始化
b = Variable(torch.from_numpy(np.asarray([0.5],dtype=np.float32)), requires_grad=True)
def linear(x):
    return x*w+b

def get_loss(y_):
    return torch.mean((y_ - y_train) ** 2)

y_ = linear(x_train)
# plt.plot(onedaydata,"bo") 
# y_p = y_.data.numpy()
# plt.plot(y_p,"r+")
# plt.show()
loss = get_loss(y_)
loss.backward()
#print('epoch: {}, loss: {},w:{},b:{},w.grad:{},b.grad:{}'.format(-1, loss.data[0],w.data[0],b.data[0],w.grad.data[0],b.grad.data[0]))  
for e in range(50000):

 
    y_ = linear(x_train)
    loss = get_loss(y_)
    w.grad.zero_() # 记得归零梯度
    b.grad.zero_() # 记得归零梯度 
      
    loss.backward()
    w.data = w.data - 0.00001 * w.grad.data
    b.data = b.data - 0.00001 * b.grad.data
    if e>5000 and e % 10000 == 0:
        y_p = y_.data.numpy()
        plt.plot(xdata,y_p,'r',linewidth=1)
        print('epoch: {}, loss: {},w:{},b:{},w.grad:{},b.grad:{}'.format(e, loss.data.item(),w.data.item(),b.data.item(),w.grad.data[0],b.grad.data[0]))
plt.plot(xdata,y_p,'g',linewidth=1)
print("expect target:",-b.data.item()/w.data.item())
y_p = y_.data.numpy()
plt.plot(xdata,y_p,'r',linewidth=4)
plt.plot(xdata,ydata,"b.") 
# y_p = y_.data.numpy()
# plt.plot(y_p,"r+")
plt.savefig("demopytorchmanual.png")






