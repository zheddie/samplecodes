import numpy as np
a = np.arange(1,101)
n_max = int(np.sqrt(len(a)))
print("n_max=",n_max)
isprime = np.ones(len(a),dtype=bool)
isprime[0]=False
for i in range(2,n_max):
	if i in a[isprime]:
		isprime[i**2-1::i]=False
print(a[isprime])
#print(a)
