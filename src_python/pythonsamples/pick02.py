import pickle
class Person:  
	def __init__(self,n,a):  
		self.name=n  
		self.age=a  
	def show(self):  
		print (self.name+"_"+str(self.age))
aa = Person("JGood", 2)  
#aa.show()  
s = pickle.dumps(aa)  
bb=pickle.loads(s)  
bb.show()  
