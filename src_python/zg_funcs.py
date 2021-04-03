import torch
from torch import nn
from torch.autograd import Variable
from datetime import datetime
import numpy as np
from sys import version_info
from torch.nn import DataParallel

import time
def get_acc(output, label):
    if version_info[0]<3:
        total = output.data.size()[0]
    else:
        total = output.shape[0]
    _, pred_label = output.max(1)
    num_correct = (pred_label == label).sum()#.data.item()
    # if version_info[0]<3:
    #     print("zg.num_correct=",num_correct.data[0],",total=",total)
    # else:
    #     print("zg.num_correct=",num_correct.data.item(),",total=",total)
    if torch.cuda.is_available():
        # return num_correct.cpu().data.numpy() / total
        return num_correct.data[0]*1.0# / total
    else:
        #return num_correct.numpy() / total
        return num_correct.data.item()*1.0# / total
        
def rnntrain(net, train_data, valid_data, num_epochs, optimizer, criterion):
    if torch.cuda.is_available():
        net = net.cuda()
    prev_time = datetime.now()
    for epoch in range(num_epochs):
        train_loss = 0
        train_acc = 0
        net = net.train()
        batchcount = 0
        for im, label in train_data:
            starttime = time.time()
            if torch.cuda.is_available():
                im = Variable(im.cuda())  # (bs, 3, h, w)
                label = Variable(label.cuda())  # (bs, h, w)
            else:
                im = Variable(im)
                label = Variable(label)
            # forward
            #print("im.size:",im.size())
            im = torch.squeeze(im,1)
            #print("im.size:",im.size(),im)
            #label.unsqueeze(0)
            # print("label.size:",label.size())
            output = net(im)
            loss = criterion(output, label)
            # backward
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            if version_info[0]<3:
                train_loss += loss.data[0]
            else:
                train_loss += loss.data.item()
            train_acc += get_acc(output, label)
            endtime = time.time()
            
            batchcount+=1
            if(batchcount % 100 == 0 or batchcount == 938):
                print("batch:{},time:{},train_acc:{}".format(batchcount,endtime-starttime,train_acc))


        cur_time = datetime.now()
        h, remainder = divmod((cur_time - prev_time).seconds, 3600)
        m, s = divmod(remainder, 60)
        time_str = "Time %02d:%02d:%02d" % (h, m, s)
        if valid_data is not None:
            valid_loss = 0
            valid_acc = 0
            net = net.eval()
            for im, label in valid_data:
                if torch.cuda.is_available():
                    im = Variable(im.cuda(), volatile=True)
                    label = Variable(label.cuda(), volatile=True)
                else:
                    if version_info[0]<3:
                        im = Variable(im, volatile=True)
                        label = Variable(label, volatile=True)
                    else:
                        with torch.no_grad():
                            im = torch.tensor(im)
                            label = torch.tensor(label)
                im = torch.squeeze(im,1)
                output = net(im)
                loss = criterion(output, label)
                if version_info[0]<3:
                    valid_loss += loss.data[0]
                else:
                    valid_loss += loss.data.item()
                valid_acc += get_acc(output, label)
            epoch_str = (
                "Epoch %d. Train Loss: %f, Train Acc: %f, Valid Loss: %f, Valid Acc: %f, "
                % (epoch, train_loss / len(train_data),
                   train_acc / len(train_data), valid_loss / len(valid_data),
                   valid_acc / len(valid_data)))
        else:
            epoch_str = ("Epoch %d. Train Loss: %f, Train Acc: %f, " %
                         (epoch, train_loss / len(train_data),
                          train_acc / len(train_data)))
        prev_time = cur_time
        print(epoch_str + time_str)

def train(net, train_data, valid_data, num_epochs, optimizer, criterion):
    if torch.cuda.is_available():
        net = net.cuda()
    prev_time = datetime.now()
    for epoch in range(num_epochs):
        train_loss = 0
        train_acc = 0
        net = net.train()
        batchcount = 0
        for im, label in train_data:
            starttime = time.time()
            if torch.cuda.is_available():
                im = Variable(im.cuda())  # (bs, 3, h, w)
                label = Variable(label.cuda())  # (bs, h, w)
            else:
                im = Variable(im)
                label = Variable(label)
            # forward
            output = net(im)
            loss = criterion(output, label)
            # backward
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            if version_info[0]<3:
                train_loss += loss.data[0]
            else:
                train_loss += loss.data.item()
            train_acc += get_acc(output, label)
            endtime = time.time()
            
            batchcount+=1
            print("batch:{},time:{},train_acc:{}".format(batchcount,endtime-starttime,train_acc))


        cur_time = datetime.now()
        h, remainder = divmod((cur_time - prev_time).seconds, 3600)
        m, s = divmod(remainder, 60)
        time_str = "Time %02d:%02d:%02d" % (h, m, s)
        if valid_data is not None:
            valid_loss = 0
            valid_acc = 0
            net = net.eval()
            for im, label in valid_data:
                if torch.cuda.is_available():
                    im = Variable(im.cuda(), volatile=True)
                    label = Variable(label.cuda(), volatile=True)
                else:
                    im = Variable(im, volatile=True)
                    label = Variable(label, volatile=True)
                output = net(im)
                loss = criterion(output, label)
                valid_loss += loss.data[0]
                valid_acc += get_acc(output, label)
            epoch_str = (
                "Epoch %d. Train Loss: %f, Train Acc: %f, Valid Loss: %f, Valid Acc: %f, "
                % (epoch, train_loss / len(train_data),
                   train_acc / len(train_data), valid_loss / len(valid_data),
                   valid_acc / len(valid_data)))
        else:
            epoch_str = ("Epoch %d. Train Loss: %f, Train Acc: %f, " %
                         (epoch, train_loss / len(train_data),
                          train_acc / len(train_data)))
        prev_time = cur_time
        print(epoch_str + time_str)
