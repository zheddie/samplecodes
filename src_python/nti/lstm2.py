import matplotlib as mlp
mlp.use("Agg")
import torch  
import torch.nn as nn  
from torch.autograd import *  
import torch.optim as optim  
import torch.nn.functional as F  
import matplotlib.pyplot as plt  
import numpy as np  

def SeriesGen(N):  
    x = torch.arange(1,N,0.01)  
    return torch.sin(x)  
  
    

def trainDataGen(seq,k):  
    dat = list()  
    L = len(seq)  
    for i in range(L-k-1):  
        indat = seq[i:i+k]  
        outdat = seq[i+1:i+k+1]  
        dat.append((indat,outdat))  
    return dat  
  
def ToVariable(x):  
    tmp = torch.FloatTensor(x)  
    return Variable(tmp)  
#import pdb; pdb.set_trace()
#y = SeriesGen(10)
# y = torch.FloatTensor([17.61,18.1,15.21,16.4,19.02,10.39,18.88,32.37,10.77,10.55,8.78,10.37,24.23,13.91,9.03,10.09,
# 10.09,10.37,9.92,11.22,10.12,10.89,10.54,10.37,10.34,10.9,10.17,9.94,23.89,9.55,23.89,22.89,8.69,
# 25.11,9.4,26.59,9.74,24.89,9.8,24.25,9.95,8.55,13.94,11.82,10.24,9.95,21.7,26.2,22.01,25.66,31.87,
# 10.62,10.83,10.94,10.94,12.74,10.36,10.66,11.01,10.5,10.18,11.95,25.6,10.67,9.78,10.45,10.38,
# 10.86,24.36,22.94,10.59,10.32,9.37,10.18,9.5]) 
y = torch.FloatTensor([10.94, 0.2, 0.23, 0.21, 0.23, 0.21, 0.22, 0.21, 0.21, 0.21, 0.22, 0.21, 0.22, 0.22, 0.23, 0.22, 0.21, 0.21, 0.22, 0.24, 0.21, 0.33, 1.17, 0.23, 10.11, 0.22, 0.23, 0.2, 0.24, 0.21, 0.24, 0.22, 0.21, 0.22, 0.22, 0.21, 0.22, 0.23, 0.22, 0.21, 0.21, 0.22, 0.21, 0.23, 0.21, 0.23, 1.08, 0.22, 10.36, 0.21, 0.24, 0.2, 0.25, 0.22, 0.31, 0.18, 0.19, 0.2, 0.21, 0.2, 0.2, 0.19, 0.2, 0.18, 0.18, 0.2, 0.19, 0.24, 0.19, 0.2, 1.07, 0.2, 10.66, 0.19, 0.2, 0.2, 0.19, 0.2, 0.21, 0.19, 0.2, 0.37, 0.22, 0.18, 0.31, 0.19, 0.2, 0.19, 0.23, 0.19, 0.22, 0.19, 0.19, 0.19, 1.05, 0.19, 11.01, 0.19, 0.19, 0.2, 0.19, 0.19, 0.2, 0.19, 0.2, 0.25, 0.2, 0.2, 0.27, 0.18, 0.21, 0.19, 0.2, 0.19, 0.2, 0.19, 0.2, 0.2, 1.07, 0.18, 10.5, 0.19, 0.19, 0.19, 0.19, 0.18, 0.21, 0.21, 0.2, 0.18, 0.19, 0.21, 0.19, 0.2, 0.19, 0.19, 0.21, 0.18, 0.22, 0.19, 0.21, 0.22, 1.29, 0.25, 10.18, 0.22, 0.21, 0.22, 0.22, 0.2, 0.25, 0.22, 0.22, 0.21, 0.23, 0.22, 0.2, 0.24, 0.22, 0.32, 0.22, 0.21, 0.23, 0.24, 0.24, 0.23, 1.41, 0.23, 11.95, 0.2, 0.24, 0.21, 0.23, 0.2, 0.22, 0.23, 0.21, 0.24, 0.21, 0.22, 0.21, 0.21, 0.24, 0.24, 0.23, 0.24, 0.24, 0.26, 0.24, 0.24, 1.05, 0.32, 11.22, 0.21, 0.24, 0.21, 0.22, 0.23, 0.97, 0.27, 0.3, 0.22, 0.22, 0.22, 0.21, 0.21, 0.22, 0.22, 0.21, 0.23, 0.22, 0.23, 0.21, 0.21, 1.08, 0.23, 10.67, 0.21, 0.21, 0.21, 0.21, 0.21, 0.22, 0.28, 0.22, 0.27, 0.24, 0.22, 0.2, 0.22, 0.23, 0.23, 0.26, 0.22, 0.22, 0.22, 0.22, 0.2, 1.04, 0.22, 9.78, 0.2, 0.2, 0.19, 0.22, 0.21, 0.22, 0.2, 0.2, 0.22, 0.21, 0.24, 0.22, 0.21, 0.22, 0.27, 0.22, 0.21, 0.23, 0.22, 0.22, 0.22, 1.06, 0.22, 10.45, 0.2, 0.22, 0.21, 0.21, 0.21, 0.22, 0.2, 0.21, 0.2, 0.22, 0.19, 0.21, 0.28, 0.22, 0.2, 0.22, 0.21, 0.23, 0.22, 0.23, 0.21, 1.03, 0.25, 10.38, 0.21, 0.24, 0.28, 0.23, 0.23, 0.26, 0.22, 0.21, 0.21, 0.22, 0.21, 0.21, 0.22, 0.21, 0.23, 0.24, 0.21, 0.23, 0.21, 0.24, 0.22, 1.02, 0.24, 10.86, 0.23, 0.24, 0.21, 0.24, 0.23, 0.22, 0.21, 0.23, 0.22, 0.22, 0.22, 0.23, 0.23, 0.22, 0.22, 0.2, 0.24, 0.24, 0.21, 0.23, 0.22, 1.0, 0.23, 10.28, 0.2, 0.22, 0.22, 0.25, 0.21, 0.22, 0.24, 0.68, 0.22, 0.22, 0.22, 0.21, 0.21, 0.22, 0.22, 0.21, 0.22, 0.24, 0.21, 0.22, 0.21, 1.78, 0.25, 10.6, 0.2, 0.28, 0.24, 0.2, 0.2, 0.27, 0.24, 0.23, 0.21, 0.2, 0.21, 0.21, 0.19, 0.2, 0.21, 0.2, 0.22, 0.2, 0.21, 0.2, 0.15, 1.04, 0.27, 10.59, 0.2, 0.21, 0.2, 0.19, 0.18, 0.23, 0.22, 0.22, 0.24, 0.2, 0.22, 0.21, 0.2, 0.21, 0.18, 0.2, 0.2, 0.21, 0.21, 0.19, 0.21, 0.98, 0.2, 10.32, 0.19, 0.2, 0.2, 0.2, 0.19, 0.21, 0.18, 0.21, 0.19, 0.2, 0.2, 0.2, 0.2, 0.23, 0.19, 0.2, 0.23, 0.2, 0.22, 0.2, 0.23, 0.96, 0.2, 9.37, 0.19, 0.22, 0.2, 0.2, 0.24, 0.2, 0.21, 0.2, 0.21, 0.21, 0.19, 0.2, 0.22, 0.21, 0.19, 0.21, 0.18, 0.22, 0.22, 0.2, 0.2, 0.94, 0.19, 10.18, 0.21, 0.2, 0.21, 0.22, 0.41, 0.41, 0.39, 0.38, 0.4, 0.41, 0.41, 0.4, 0.21, 0.2, 0.2, 0.21, 0.18, 0.22, 0.19, 0.2, 0.19, 0.93, 0.2, 9.5, 0.18, 0.21, 0.19, 0.21, 0.21, 0.23, 0.21, 0.19, 0.23, 0.22, 0.2, 0.21, 0.2, 0.23, 0.21, 0.2, 0.19, 0.33, 0.2, 0.2, 0.2, 0.98, 0.21, 10.58, 0.19, 0.21, 0.2, 0.19, 0.21, 0.21, 0.2, 0.18, 0.2, 0.22, 0.2, 0.21, 0.2, 0.21, 0.21, 0.21, 0.25, 0.21, 0.2, 0.21, 0.2, 0.92, 0.21,10.16, 0.22, 0.2, 0.2, 0.2, 0.19, 0.2, 0.21, 0.21, 0.2, 0.21, 0.21, 0.19, 0.21, 0.21, 0.2, 0.19, 0.2, 0.22, 0.2, 0.2, 0.19, 0.86, 0.22, 10.24, 0.2, 0.22, 0.14, 0.26, 0.25, 0.26, 0.24, 0.24, 0.27, 0.24, 0.47, 0.24, 0.23, 0.25, 0.24, 0.19, 0.22, 0.22, 0.21, 0.21, 0.22, 0.88, 0.21, 10.79, 0.2, 0.21, 0.21, 0.2, 0.21, 0.22, 0.21, 0.2, 0.2, 0.21, 0.2, 0.2, 0.2, 0.21, 0.22, 0.21, 0.24, 0.21, 0.22, 0.2, 0.2, 0.86, 0.22, 10.34, 0.21, 0.75, 0.21, 0.18, 0.16, 0.19, 0.17, 0.16, 0.23, 0.16, 0.16, 0.16, 0.16, 0.18, 0.17, 0.16, 0.16, 0.17, 0.17, 0.16, 0.16, 0.8, 0.16, 10.53, 0.18, 0.17, 0.16, 0.16, 0.16, 0.17, 0.16, 0.16, 0.16, 0.17, 0.17, 0.15, 0.15, 0.17, 0.16, 0.17, 0.15, 0.16, 0.15, 0.17, 0.16, 0.79, 0.15, 11.73, 0.16, 0.16, 0.18, 0.16, 0.16, 0.17, 0.16, 0.19, 0.18, 0.17, 0.16, 0.17, 0.15, 0.17, 0.16, 0.16, 0.15, 0.17, 0.16, 0.18, 0.16, 0.82, 0.16, 10.98, 0.79, 0.18, 0.2, 0.2, 0.14, 0.14, 0.15, 0.2, 0.2, 0.21, 0.2, 0.2, 0.21, 0.31, 0.19, 0.21, 0.2, 0.22, 0.19, 0.22, 0.2, 0.87, 0.2, 10.4, 0.18, 1.49, 0.2, 0.2, 0.2, 0.21, 0.19, 0.19, 0.21, 0.21, 0.22, 0.2, 0.2, 0.21, 0.21, 0.19, 0.2, 0.19, 0.22, 0.19, 0.21, 0.94, 0.2, 12.29, 0.22, 0.21, 0.2, 0.2, 0.19, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.21, 0.21, 0.19, 0.2, 0.2, 0.45, 0.21, 0.2, 0.21, 0.92, 0.22, 9.77, 0.19, 0.25, 0.2, 0.2, 0.19, 0.22, 0.2, 0.21, 0.23, 0.21, 0.19, 0.22, 0.2, 0.2, 0.2, 0.2, 0.2, 0.34, 0.19, 0.21, 0.21, 1.07, 0.18, 10.7, 0.19, 0.24, 0.19, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.21, 0.18, 0.19, 0.21, 0.22, 0.19, 0.21, 0.21, 0.21, 0.19, 0.2, 0.19, 0.91, 0.2])
#Total 792 data.
dat = trainDataGen(y.numpy(),10)  
  
print(dat,len(dat))
class LSTMpred(nn.Module):  
  
    def __init__(self,input_size,hidden_dim):  
        super(LSTMpred,self).__init__()  
        self.input_dim = input_size  
        self.hidden_dim = hidden_dim  
        self.lstm = nn.LSTM(input_size,hidden_dim)  
        self.hidden2out = nn.Linear(hidden_dim,1)  
        self.hidden = self.init_hidden()  
  
    def init_hidden(self):  
        return (Variable(torch.zeros(1, 1, self.hidden_dim)),  
                Variable(torch.zeros(1, 1, self.hidden_dim)))  
  
    def forward(self,seq):  
        lstm_out, self.hidden = self.lstm(  
            seq.view(len(seq), 1, -1), self.hidden)  
        outdat = self.hidden2out(lstm_out.view(len(seq),-1))  
        return outdat  
  
  
# model = LSTMpred(1,6)  
model = LSTMpred(1,6) 
loss_function = nn.MSELoss()  
optimizer = optim.SGD(model.parameters(), lr=0.01)  
losses = []
for epoch in range(10):  
    print(epoch)  
    for seq, outs in dat[:733]:  
    #for seq, outs in dat[:700]: 
        seq = ToVariable(seq)  
        outs = ToVariable(outs)  
        #outs = torch.from_numpy(np.array([outs]))  
  
        optimizer.zero_grad()  
  
        model.hidden = model.init_hidden()  
  
        modout = model(seq)  
        outs.resize_(modout.size()[0], 1)

        loss = loss_function(modout, outs)  
        #print("seq=",seq)
        #print("modout=",modout)
        #print("outs=",outs)
        #print("loss=",loss)
        losses.append(loss)
        loss.backward()  
        optimizer.step()  
  
predDat = []  
for seq, trueVal in dat[733:]:  
#for seq, trueVal in dat[700:]: 
    seq = ToVariable(seq)  
    trueVal = ToVariable(trueVal)  
    pred = model(seq)[-1].data.numpy()
    print("pred=",pred)
    predDat.append(pred[0])  
  
  
#fig = plt.figure()
plt.subplot(2,1,1)  
plt.title("Data and predict")
plt.ylabel("CPU Percentage(%)")
plt.xlabel("Hours")
plt.plot(range(0,792),y.numpy(),"bo")  
plt.plot(range(744,792),predDat,"r+")
plt.subplot(2,1,2)
plt.title("Loss trend.")
plt.ylabel("Loss Values")
plt.xlabel("Iteration Count")
plt.plot(range(10*733),losses,"g")  
# plt.plot(range(900),y.numpy(),"bo")  
# plt.plot(range(711,900),predDat,"r+")  
# plt.plot(range(10*700),losses,"g")  
#plt.show()
plt.savefig("savelstm.png")
