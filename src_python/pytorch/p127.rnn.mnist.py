import torch
from torch import nn,optim
from torch.utils.data import  DataLoader
from torchvision import datasets, transforms
from zg_funcs import rnntrain

class RnnLstm(nn.Module):
    def __init__(self,in_dim,hidden_dim,n_layer,n_class):
        super(RnnLstm,self).__init__()
        self.n_layer  = n_layer
        self.hidden_dim  = hidden_dim
        self.lstm = nn.LSTM(in_dim,hidden_dim,n_layer,batch_first=True)
        self.classifier = nn.Linear(hidden_dim,n_class)
    def forward(self,x):
        out,_ = self.lstm(x)
        #print("zg.forward.cp0,",out.size())
        out = out[:,-1,:]
        #print("zg.forward.cp1,",out.size())
        out = self.classifier(out)
        #print("zg.forward.cp2,",out.size())
        #print("zg.forward.cp3,",out[-1,:])
        
        return out

batch_size = 64
learning_rate = 1e-2
num_epoch = 1

if __name__ == '__main__':
    data_tf = transforms.Compose(
        [
        transforms.ToTensor(),
        transforms.Normalize([0.5],[0.5])
        ])
    train_dataset = datasets.MNIST(root='./data',train=True,transform=data_tf,download=True)
    test_dataset = datasets.MNIST(root='./data',train=False,transform=data_tf,download=True)
    train_loader = DataLoader(train_dataset,batch_size=batch_size,shuffle=True)
    test_loader = DataLoader(test_dataset,batch_size=batch_size,shuffle=False)
    model = RnnLstm(28,250,1,10)
    criterion = nn.CrossEntropyLoss()
    optimizer = optim.SGD(model.parameters(),lr=learning_rate)
    rnntrain(model,train_loader,test_loader,num_epoch,optimizer,criterion)

