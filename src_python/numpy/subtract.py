import numpy as np
x1 = np.arange(9)
x2 = np.arange(9.0).reshape(3,3)
x3 = np.arange(3.0)
x4 = np.full((4,4),255)
print x1
print x2
print x3
print np.subtract(x2,x3)
print x4
