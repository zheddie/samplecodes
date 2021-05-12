# -*- coding: UTF-8 -*-

import torch
from torch import nn
from torch.autograd import Variable
import torch.nn.functional as F
from torchvision.datasets import CIFAR10
import numpy as np
from  PIL import Image
from sys import version_info
from torch.nn import DataParallel
from zg_funcs import train


global totalcount
totalcount = 0
def conv3x3(in_channel, out_channel, stride=1):
    return nn.Conv2d(in_channel, out_channel, 3, stride=stride, padding=1, bias=False)

class residual_block(nn.Module):
    def __init__(self, in_channel, out_channel, same_shape=True):
        super(residual_block, self).__init__()
        self.same_shape = same_shape
        stride=1 if self.same_shape else 2
        
        self.conv1 = conv3x3(in_channel, out_channel, stride=stride)
        self.bn1 = nn.BatchNorm2d(out_channel)
        
        self.conv2 = conv3x3(out_channel, out_channel)
        self.bn2 = nn.BatchNorm2d(out_channel)
        if not self.same_shape:
            self.conv3 = nn.Conv2d(in_channel, out_channel, 1, stride=stride)
        
    def forward(self, x):
        out = self.conv1(x)
        out = F.relu(self.bn1(out), True)
        out = self.conv2(out)
        out = F.relu(self.bn2(out), True)
        
        if not self.same_shape:
            x = self.conv3(x)
        return F.relu(x+out, True)    

class resnet(nn.Module):
    def __init__(self, in_channel, num_classes, verbose=False):
        super(resnet, self).__init__()
        self.verbose = verbose
        
        self.block1 = nn.Conv2d(in_channel, 64, 7, 2)   #b,64,45,45 : (96-7)/2+1 =~45
        
        self.block2 = nn.Sequential(
            nn.MaxPool2d(3, 2),   #b,64,22,22         : (45-3)/2 +1 = 22
            residual_block(64, 64), #b,64,22,22
            residual_block(64, 64)   #b,64,22,22                 
        )
        
        self.block3 = nn.Sequential(
            residual_block(64, 128, False), #b,128,11,11 : (22-3+2)/2+1 = 11
            residual_block(128, 128) #b,128,11,11
        )
        
        self.block4 = nn.Sequential(
            residual_block(128, 256, False), #b,256,6,6  :(11-3+2)/2+1 = 6
            residual_block(256, 256)#b,256,6,6
        )
        
        self.block5 = nn.Sequential(
            residual_block(256, 512, False), #b,512,3,3 : (6-3+2)/2+1 = 3
            residual_block(512, 512),#b,512,3,3
            nn.AvgPool2d(3) #b,512,1,1                  :3/3=1
        )
        
        self.classifier = nn.Linear(512, num_classes)
        #params = 7*7*64+3*3*64+3*3*128+3*3*512+512*10 =14592 << 96*96*3*10=276480
    def forward(self, x):
        x = self.block1(x)
        if self.verbose:
            if version_info[0] < 3:
                print('block 1 output: {}'.format(x.data.shape))
            else:
                print('block 1 output: {}'.format(x.shape))
        x = self.block2(x)
        if self.verbose:
            if version_info[0] < 3:
                print('block 2 output: {}'.format(x.data.shape))
            else:
                print('block 2 output: {}'.format(x.shape))
        x = self.block3(x)
        if self.verbose:
            if version_info[0] < 3:
                print('block 3 output: {}'.format(x.data.shape))
            else:
                print('block 3 output: {}'.format(x.shape))
        x = self.block4(x)
        if self.verbose:
            if version_info[0] < 3:
                print('block 4 output: {}'.format(x.data.shape))
            else:
                print('block 4 output: {}'.format(x.shape))
        x = self.block5(x)
        if self.verbose:
            if version_info[0] < 3:
                print('block 5 output: {}'.format(x.data.shape))
            else:
                print('block 5 output: {}'.format(x.shape))
        if version_info[0] < 3:
            x = x.view(x.size()[0], -1)
        else:
            x = x.view(x.shape[0], -1)
        x = self.classifier(x)
        return x




def data_tf(x):
    #x is PIL.Image.Image
    global totalcount
    totalcount+=1
    if(totalcount == 1):
        print(totalcount,x.format, x.size, x.mode)
        print("PIL.Image.Image.NEAREST",Image.NEAREST)
        print("PIL.Image.BILINEAR",Image.BILINEAR)
        x.save("cifar10.pic01.32.bmp")

        # print(list(x.getdata()))
    x = x.resize((96, 96), 2) # 将图片放大到 96 x 96,"2"是一种resize的策略，PIL.Image.BILINEAR。
    if(totalcount == 1):
        x.save("cifar10.pic01.96.bmp")
    x = np.array(x, dtype='float32') / 255
    x = (x - 0.5) / 0.5 # 标准化，这个技巧之后会讲到
    x = x.transpose((2, 0, 1)) # 将 channel 放到第一维，只是 pytorch 要求的输入方式
    x = torch.from_numpy(x)
    return x
     
#train_set = CIFAR10('./data', train=True, transform=data_tf,download=True)
train_set = CIFAR10('./data', train=True, transform=data_tf,download=False)
train_data = torch.utils.data.DataLoader(train_set, batch_size=64, shuffle=True)
#test_set = CIFAR10('./data', train=False, transform=data_tf,download=True)
test_set = CIFAR10('./data', train=False, transform=data_tf,download=False)
test_data = torch.utils.data.DataLoader(test_set, batch_size=128, shuffle=False)

if torch.cuda.is_available():
    net = DataParallel(resnet(3, 10))
    net.cuda()
else:
    net = resnet(3, 10)
optimizer = torch.optim.SGD(net.parameters(), lr=0.01)
criterion = nn.CrossEntropyLoss()
#检测一下模型的有效性。
# test_net = resnet(3, 10, True)
# test_x = Variable(torch.zeros(1, 3, 96, 96))
# test_y = test_net(test_x)
# print('output: {}'.format(test_y.shape))
train(net, train_data, test_data, 20, optimizer, criterion)
torch.save(net,"p107.cifa10.pth")




