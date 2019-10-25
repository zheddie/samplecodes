from sklearn import linear_model
import numpy as np
X = np.c_[ .5, 1].T
y = [.5, 1]
test = np.c_[ 0, 2].T
regr = linear_model.LinearRegression()

import matplotlib.pyplot as plt
plt.figure()

np.random.seed(0)
for _ in range(1):
   this_X = .1*np.random.normal(size=(2, 1)) + X
   print("this_X:"+str(this_X)+"\ny:"+str(y))
   regr.fit(this_X, y)
   ptest = regr.predict(test)
   plt.plot(test,ptest )
   print("test:"+str(test)+"\nptest:"+str(ptest))
   plt.scatter(this_X, y, s=3)
plt.show()
