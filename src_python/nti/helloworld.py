#!/usr/bin/env python3
################################
# import pandas and scikit learn
################################
#import pandas as pd
import numpy as np
from sklearn import linear_model

################################
# load dataset
################################
sqfeet = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]).reshape(-1,1)
price = np.array([11.9, 12.6, 13.3, 15, 16, 16.3, 16.9, 18.2, 20.1, 20.9])

################################
# train the model
################################
model = linear_model.LinearRegression()
model.fit(sqfeet, price)

################################
# make prediction
################################
prd = model.predict( np.array([7.5]).reshape(-1,1))
print("predict of 7.5 is:",prd)
