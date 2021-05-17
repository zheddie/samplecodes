#!env python3
import numpy as np
#import scipy as sp
import matplotlib.pyplot as plt
zs = np.array([1,2,3,5,7,11,13])
#zsb = npp.array([[1,1],[2,1],[3,1],[4,3/4],[5,4/5],[6,4/6],[7,5/7],[8,5/8],[9,5/9],[10,5/10],[11,6/11],[2,1],[2,1])
zsb = np.array([[1,1]],dtype=float)
#zsbt = np.array()
def iszs(s):
	mid = int(s/2)+1
	zsbool = (zs<mid)
	zsl = zs[zsbool]
	for j in range(1,len(zsl)):
		if( s%zs[j] == 0):
			return False
	return True

for i in range(2,10000):
	#Try to expend zs.
	if(i>zs[-1]):
		if(iszs(i)):
			zs = np.append(zs,[i])

	cntzsbool = (zs<=i)
	cntzs = cntzsbool[cntzsbool]
	#cntzs = (zs<=i).nonzero()
	

	zsb = np.append(zsb,[[i,float(len(cntzs)/i)]],axis=0)
print(zs)
print(str(zsb))
yb = np.array([])
for ya in range(2,10000):
	yb = np.append(yb,[float(1/np.log(ya))],axis=0)
#plt.scatter(np.hsplit(zsb,2)[0],np.hsplit(zsb,2)[1])
#plt.plot(np.hsplit(zsb,2)[0],np.hsplit(zsb,2)[1],color="r")
plt.plot(np.arange(1,10000),np.hsplit(zsb,2)[1],color="r")
plt.plot(np.arange(2,10000),yb,color="b")

plt.show()