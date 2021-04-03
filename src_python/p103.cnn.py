import torch 
from torch import nn, optim
from torch.autograd import Variable
from torchvision import datasets,transforms
from torch.utils.data import DataLoader

class CNN(nn.Module):
	def __init__(self):
		super(CNN,self).__init__()
		self.layer1 = nn.Sequential(
			nn.Conv2d(1,16,kernel_size=3), #b 16 26 26
			nn.BatchNorm2d(16),
			nn.ReLU(inplace=True)
			)
		self.layer2 = nn.Sequential(
			nn.Conv2d(16,32,kernel_size=3), #b 32 24 24
			nn.BatchNorm2d(32),
			nn.ReLU(inplace=True),
			nn.MaxPool2d(kernel_size=2,stride=2) #b 32 12 12
			)
		self.layer3 = nn.Sequential(
			nn.Conv2d(32,64,kernel_size=3), #b 64 10 10
			nn.BatchNorm2d(64),
			nn.ReLU(inplace=True)
			)	
		self.layer4 = nn.Sequential(
			nn.Conv2d(64,128,kernel_size=3), #b 128 8 8
			nn.BatchNorm2d(128),
			nn.ReLU(inplace=True),
			nn.MaxPool2d(kernel_size=2,stride=2) # b 128 4 4
			)
		self.fc = nn.Sequential(
			nn.Linear(128*4*4,1024)
			, nn.ReLU(inplace=True)
			, nn.Linear(1024,128)
			, nn.ReLU(inplace=True)	
			, nn.Linear(128,10)
			#, nn.ReLU(inplace=True)
			)
	def forward(self,x):

		x = self.layer1(x)
#		print(x.shape)
		x = self.layer2(x)
#		print(x.shape)
		x = self.layer3(x)
#		print(x.shape)
		x = self.layer4(x)
#		print(x.shape)
		x = x.view(x.size(0),-1)
#		print(x.shape)
		x = self.fc(x)
#		print(x.shape)
		return(x)

def verifysize(mod):
	xin = Variable(torch.randn([1,1,28,28]))
	yout = mod(xin)
	#print("Input:",xin)
	print("Output:",yout)
def train(mod,trainloader,lentrain,testloader,lentest,totalimg=1000):
	mod.train()
	learning_rate = 1e-2
	num_epoches = 5
	criterion = nn.CrossEntropyLoss()
	optimizer = optim.SGD(mod.parameters(),lr=learning_rate)
	for e in range(num_epoches):
		train_loss = 0
		train_acc = 0
		totalimages = 0
		for img,label in trainloader:
			totalimages+=img.size(0)
			if totalimages > totalimg:
				break
			#print(img.shape)
			#img = img.view(img.size(0),-1)
			img = Variable(img)
			label = Variable(label)
			out = mod(img)
			loss = criterion(out,label)
			train_loss +=loss.item()*label.size(0)
			_,pred  = torch.max(out,1)
			num_correct = (pred == label).sum()
			train_acc +=num_correct.item()
			optimizer.zero_grad()
			loss.backward()
			optimizer.step()
		mod.eval()
		eval_loss = 0
		eval_acc = 0
		criterion = nn.CrossEntropyLoss()

		for img,label in testloader:
			#img = img.view(img.size(0),-1)
			img = Variable(img)
			label = Variable(label)
			out = mod(img)
			loss = criterion(out,label)
			eval_loss +=loss.item()*label.size(0)
			_,pred  = torch.max(out,1)
			num_correct = (pred == label).sum()
			eval_acc +=num_correct.item()

		print("epoch:{},train_loss:{:.6f},train_acc:{:.6f},eval_loss:{:.6f},eval_acc:{:.6f}".format(e,train_loss/totalimg,train_acc/totalimg,eval_loss/lentest,eval_acc/lentest))
			
def inference(mod,img):
	mod.eval()
	pred = mod(img)
	print("pred:",pred)
def main():
	batch_size = 64
	cnnmodel = CNN()
	verifysize(cnnmodel)

	data_tf  = transforms.Compose(
		[transforms.ToTensor(),
		transforms.Normalize([0.5],[0.5])
		]
		)

	train_dataset = datasets.MNIST(root="./data",train=True,transform=data_tf,download=True)
	test_dataset = datasets.MNIST(root="./data",train=False,transform=data_tf)
	train_loader = DataLoader(train_dataset,batch_size=batch_size,shuffle=True)
	test_loader = DataLoader(test_dataset,batch_size = batch_size, shuffle=False)
	train(cnnmodel,train_loader,len(train_dataset),test_loader,len(test_dataset),totalimg=len(train_dataset))
	#inference
	cnnmodel.eval()
	inflist = [5213,432,24358,43212,59999]
	for idx in inflist:
		img,label = train_dataset.__getitem__(idx)
		img = Variable(img)
		label = Variable(label)
		#torch.reshape(img,(1,1,28,28))
		img = img.view(1,1,28,28)
		print(img.shape)
		pred = cnnmodel(img)
		#pred1=torch.max(pred,1)
		print("img.label=",label.item(),",pred=",pred)
	print("Goodbye.")
if __name__ == '__main__':
	main()