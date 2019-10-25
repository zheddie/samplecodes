import torch 
from torch import nn
from torch.autograd import Variable
def printall(data,msg,printdata=True):
    
    print("====={}================".format(msg))
    print(data.size())
    if printdata:
        print(data)

if __name__ == '__main__':    
    #model = nn.RNN(input_size=20,hidden_size=50,num_layers=3)
    model = nn.LSTM(input_size=20,hidden_size=50,num_layers=3)
    toy_input = Variable(torch.randn(100,32,20))
    h_0 = Variable(torch.randn(3,32,50))
    c_0 = Variable(torch.randn(3,32,50))
    toy_output,(h_1,c_1) = model(toy_input,(h_0,c_0))

    # print(toy_output.size())
    # print(h_1.size())
    # print(model.weight_ih_l0)
    printall(toy_input,"toy_input",False)
    printall(h_0,"h_0",False)
    
    printall(model.weight_ih_l0,".weight_ih_l0",False)
    printall(model.bias_ih_l0,"model.bias_ih_l0",False)


    printall(model.weight_hh_l0,"model.weight_hh_l0",False)
    printall(model.bias_hh_l0,"model.bias_hh_l0",False)

    printall(model.weight_ih_l1,"model.weight_ih_l1",False)
    printall(model.bias_ih_l1,"model.bias_ih_l1",False)

    printall(model.weight_hh_l1,"model.weight_hh_l1",False)
    printall(model.bias_hh_l1,"model.bias_hh_l1",False)

    printall(model.weight_ih_l2,"model.weight_ih_l2",False)
    printall(model.bias_ih_l2,"model.bias_ih_l2",False)

    printall(model.weight_hh_l2,"model.weight_hh_l2",False)
    printall(model.bias_hh_l2,"model.bias_hh_l2",False)

    printall(toy_output,"toy_output",False)
    printall(h_1,"h_1",False)
    printall(c_1,"c_1",False)