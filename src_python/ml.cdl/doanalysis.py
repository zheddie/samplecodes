import pandas as pd
import torch
from torch import nn,optim
from torch.utils.data import Dataset,DataLoader
import numpy as np
from torchvision import transforms
import time
use_cuda = torch.cuda.is_available()
class pumpDataset(Dataset):
    def __init__(self,csv_file,label_index,transform=None):
        datasets_pd = pd.read_csv(csv_file)
        ds = datasets_pd.iloc[:,1:].values
        ds = ds.astype('float32')
        ds_shape = ds.shape
        #print("zg.datasets.cp0.ds_shape=",ds_shape)
        for i in range(ds_shape[1]):
            ds_mean  = np.mean(ds[:,i])
            ds_std = np.std(ds[:,i])
            #print("zg.datasets.cp1.ds_mean=",ds_mean,"ds_std=",ds_std)
            for j in range(ds_shape[0]):
                #print("zg.datasets.cp2.ds[",j,",",i,"]=",ds[j,i])
                ds[j,i] = (ds[j,i]-ds_mean)/ds_std
                #print("zg.datasets.cp2.ds[",j,",",i,"]=",ds[j,i])
                
        #print("ds:",type(ds),ds.shape)
        self.datasets=ds
        self.label_index = label_index
        self.transform = transform
    def __len__(self):
        return(len(self.datasets))
    def __getitem__(self,idx):

        dataall = self.datasets[idx,:]
        datalen = len(dataall)
        #print("zg.cp0.dataall=",dataall,",len=",datalen)
        idxarr = np.arange(datalen)
        idxarr = np.delete(idxarr,self.label_index)
        #print("zg.cp1.idxarr=",idxarr)
        data = self.transform(self.datasets[idx,idxarr])
        label =self.transform(np.array([self.datasets[idx,self.label_index]],np.float))
        #print("zg.cp2.data=",data,",label=",label)
        return(data,label)

class pumpDatasetAll(Dataset):
    def __init__(self,csv_file,transform=None):
        datasets_pd = pd.read_csv(csv_file)
        ds = datasets_pd.iloc[:,1:].values
        ds = ds.astype('float32')
        #print("zg.ds:",type(ds))
        ds_shape = ds.shape
        #print("zg.datasets.cp0.ds_shape=",ds_shape)
        for i in range(ds_shape[1]):
            ds_mean  = np.mean(ds[:,i])
            ds_std = np.std(ds[:,i])
            #print("zg.datasets.cp1.ds_mean=",ds_mean,"ds_std=",ds_std)
            for j in range(ds_shape[0]):
                #print("zg.datasets.cp2.ds[",j,",",i,"]=",ds[j,i])
                ds[j,i] = (ds[j,i]-ds_mean)/ds_std
                #print("zg.datasets.cp2.ds[",j,",",i,"]=",ds[j,i])
                
        #print("ds:",type(ds),ds.shape)
        self.datasets=ds
        self.datalen = len(ds)
        self.transform = transform
    def __len__(self):
        return self.datalen
    def __getitem__(self,idx):
        #print("zg.cp1.idxarr=",idxarr)
        data = self.transform(self.datasets[idx,:])
        if idx == self.datalen-1:
            label = torch.FloatTensor([])
        else:
            label = self.transform(self.datasets[idx+1,:])
        #print("zg.cp2.data=",data,",label=",label)
        return(data,label)

class trs_toTensor(object):
    def __call__(self,nd_array):
        return torch.from_numpy(nd_array)
class RelateRNN(nn.Module):
    def __init__(self,inputsize,hiddensize,outputsize):
        super(RelateRNN,self).__init__()
        self.gru  = nn.GRU(inputsize,hiddensize,num_layers=1)
        self.line = nn.Linear(hiddensize,outputsize)
        self.hidden_size = hiddensize
    def forward(self,x,hidden):
        #print("zg.forward().cp0.x:{},hidden:{}".format(x.size(),hidden.size()))
        x,hidden = self.gru(x,hidden)
        #print("zg.forward().cp1.x:{},hidden:{}".format(x.size(),hidden.size()))
        x = self.line(x)
        #print("zg.forward().cp2.x:{},hidden:{}".format(x.size(),hidden.size()))
        return x,hidden
    def initHidden(self):
        result = torch.zeros(1,1, self.hidden_size,dtype=torch.float)
        
        if use_cuda:
            return result.cuda()
        else:
            return result        
def train(model,epoch,loader):
    #criterion = nn.CrossEntropyLoss()
    criterion = nn.MSELoss()
    optimizer = optim.Adam(model.parameters(),lr=2e-2)
    if use_cuda:
        criterion  = criterion.cuda()
        optimizer = optimizer.cuda()
    for j in range(epoch):
        hid = model.initHidden()

        total_loss = 0
        since = time.time()
        for i,one_data in enumerate(loader):
            data,label = one_data
            if label.size()[0] == 0:
                break
            if use_cuda:
                data = data.cuda()
                label = label.cuda()
            #print(data,label)
            data = torch.unsqueeze(data,0)
            label = torch.unsqueeze(label,0)
            
            #print("SIZE:data:{},label:{},hid:{}".format(data.size(),label.size(),hid.size()))
            #print("TYPE:data:{},label:{},hid:{}".format(data.dtype,label.dtype,hid.dtype))
            
            pred,hid = model(data,hid)
            #print("TYPE:pred:{},label:{}".format(pred.dtype,label.dtype))
            #print("i={}:data:{},label:{},pred:{}".format(i,data,label,pred))
            loss = criterion(pred,label)
            total_loss+=loss.data.item()
            # backward
            optimizer.zero_grad()
            #loss.backward(retain_graph=True)
            loss.backward(retain_graph=True)
            optimizer.step()
            if ( i%100 == 0):
                tmp=  time.time()
                during = tmp - since
                since = tmp
                print("epoch:{},idx:{},loss={}:during={}".format(j,i,total_loss,during))
        print("epoch:{}/{},loss={}".format(j,epoch,total_loss))



if __name__== '__main__':
    trans = transforms.Compose([trs_toTensor()])
    pumpds = pumpDatasetAll("water-pump-data.csv",trans)
    pump_dataloader = DataLoader(pumpds, shuffle=False)
    mrelate = RelateRNN(10,20,10)
    if use_cuda:
        mrelate = mrelate.cuda()
    train(mrelate,2,pump_dataloader)

        #rint("data=",data,",label=",label)
    print(mrelate.parameters())

