import numpy as np
from pandas import DataFrame
from pandas import concat
from sklearn import metrics
from sklearn.svm import SVR
import matplotlib.pyplot as plt
# transform time series data to supervised learning data
def timeSeries_to_supervised(data, lag=1):
    df = DataFrame(data)
    #columns = [df.shift(i) for i in range(1,lag+1)]
    columns = [df.shift(i) for i in range(lag,0,-1)]
    columns.append(df)
    df = concat(columns, axis=1)
    df.fillna((data[0]+data[1])/2, inplace=True)
    return df.values

#all the data
data = [10.94, 0.2, 0.23, 0.21, 0.23, 0.21, 0.22, 0.21, 0.21, 0.21, 0.22, 0.21, 0.22, 0.22, 0.23, 0.22, 0.21, 0.21, 0.22, 0.24, 0.21, 0.33, 1.17, 0.23, 10.11, 0.22, 0.23, 0.2, 0.24, 0.21, 0.24, 0.22, 0.21, 0.22, 0.22, 0.21, 0.22, 0.23, 0.22, 0.21, 0.21, 0.22, 0.21, 0.23, 0.21, 0.23, 1.08, 0.22, 10.36, 0.21, 0.24, 0.2, 0.25, 0.22, 0.31, 0.18, 0.19, 0.2, 0.21, 0.2, 0.2, 0.19, 0.2, 0.18, 0.18, 0.2, 0.19, 0.24, 0.19, 0.2, 1.07, 0.2, 10.66, 0.19, 0.2, 0.2, 0.19, 0.2, 0.21, 0.19, 0.2, 0.37, 0.22, 0.18, 0.31, 0.19, 0.2, 0.19, 0.23, 0.19, 0.22, 0.19, 0.19, 0.19, 1.05, 0.19, 11.01, 0.19, 0.19, 0.2, 0.19, 0.19, 0.2, 0.19, 0.2, 0.25, 0.2, 0.2, 0.27, 0.18, 0.21, 0.19, 0.2, 0.19, 0.2, 0.19, 0.2, 0.2, 1.07, 0.18, 10.5, 0.19, 0.19, 0.19, 0.19, 0.18, 0.21, 0.21, 0.2, 0.18, 0.19, 0.21, 0.19, 0.2, 0.19, 0.19, 0.21, 0.18, 0.22, 0.19, 0.21, 0.22, 1.29, 0.25, 10.18, 0.22, 0.21, 0.22, 0.22, 0.2, 0.25, 0.22, 0.22, 0.21, 0.23, 0.22, 0.2, 0.24, 0.22, 0.32, 0.22, 0.21, 0.23, 0.24, 0.24, 0.23, 1.41, 0.23, 11.95, 0.2, 0.24, 0.21, 0.23, 0.2, 0.22, 0.23, 0.21, 0.24, 0.21, 0.22, 0.21, 0.21, 0.24, 0.24, 0.23, 0.24, 0.24, 0.26, 0.24, 0.24, 1.05, 0.32, 11.22, 0.21, 0.24, 0.21, 0.22, 0.23, 0.97, 0.27, 0.3, 0.22, 0.22, 0.22, 0.21, 0.21, 0.22, 0.22, 0.21, 0.23, 0.22, 0.23, 0.21, 0.21, 1.08, 0.23, 10.67, 0.21, 0.21, 0.21, 0.21, 0.21, 0.22, 0.28, 0.22, 0.27, 0.24, 0.22, 0.2, 0.22, 0.23, 0.23, 0.26, 0.22, 0.22, 0.22, 0.22, 0.2, 1.04, 0.22, 9.78, 0.2, 0.2, 0.19, 0.22, 0.21, 0.22, 0.2, 0.2, 0.22, 0.21, 0.24, 0.22, 0.21, 0.22, 0.27, 0.22, 0.21, 0.23, 0.22, 0.22, 0.22, 1.06, 0.22, 10.45, 0.2, 0.22, 0.21, 0.21, 0.21, 0.22, 0.2, 0.21, 0.2, 0.22, 0.19, 0.21, 0.28, 0.22, 0.2, 0.22, 0.21, 0.23, 0.22, 0.23, 0.21, 1.03, 0.25, 10.38, 0.21, 0.24, 0.28, 0.23, 0.23, 0.26, 0.22, 0.21, 0.21, 0.22, 0.21, 0.21, 0.22, 0.21, 0.23, 0.24, 0.21, 0.23, 0.21, 0.24, 0.22, 1.02, 0.24, 10.86, 0.23, 0.24, 0.21, 0.24, 0.23, 0.22, 0.21, 0.23, 0.22, 0.22, 0.22, 0.23, 0.23, 0.22, 0.22, 0.2, 0.24, 0.24, 0.21, 0.23, 0.22, 1.0, 0.23, 10.28, 0.2, 0.22, 0.22, 0.25, 0.21, 0.22, 0.24, 0.68, 0.22, 0.22, 0.22, 0.21, 0.21, 0.22, 0.22, 0.21, 0.22, 0.24, 0.21, 0.22, 0.21, 1.78, 0.25, 10.6, 0.2, 0.28, 0.24, 0.2, 0.2, 0.27, 0.24, 0.23, 0.21, 0.2, 0.21, 0.21, 0.19, 0.2, 0.21, 0.2, 0.22, 0.2, 0.21, 0.2, 0.15, 1.04, 0.27, 10.59, 0.2, 0.21, 0.2, 0.19, 0.18, 0.23, 0.22, 0.22, 0.24, 0.2, 0.22, 0.21, 0.2, 0.21, 0.18, 0.2, 0.2, 0.21, 0.21, 0.19, 0.21, 0.98, 0.2, 10.32, 0.19, 0.2, 0.2, 0.2, 0.19, 0.21, 0.18, 0.21, 0.19, 0.2, 0.2, 0.2, 0.2, 0.23, 0.19, 0.2, 0.23, 0.2, 0.22, 0.2, 0.23, 0.96, 0.2, 9.37, 0.19, 0.22, 0.2, 0.2, 0.24, 0.2, 0.21, 0.2, 0.21, 0.21, 0.19, 0.2, 0.22, 0.21, 0.19, 0.21, 0.18, 0.22, 0.22, 0.2, 0.2, 0.94, 0.19, 10.18, 0.21, 0.2, 0.21, 0.22, 0.41, 0.41, 0.39, 0.38, 0.4, 0.41, 0.41, 0.4, 0.21, 0.2, 0.2, 0.21, 0.18, 0.22, 0.19, 0.2, 0.19, 0.93, 0.2, 9.5, 0.18, 0.21, 0.19, 0.21, 0.21, 0.23, 0.21, 0.19, 0.23, 0.22, 0.2, 0.21, 0.2, 0.23, 0.21, 0.2, 0.19, 0.33, 0.2, 0.2, 0.2, 0.98, 0.21, 10.58, 0.19, 0.21, 0.2, 0.19, 0.21, 0.21, 0.2, 0.18, 0.2, 0.22, 0.2, 0.21, 0.2, 0.21, 0.21, 0.21, 0.25, 0.21, 0.2, 0.21, 0.2, 0.92, 0.21,10.16, 0.22, 0.2, 0.2, 0.2, 0.19, 0.2, 0.21, 0.21, 0.2, 0.21, 0.21, 0.19, 0.21, 0.21, 0.2, 0.19, 0.2, 0.22, 0.2, 0.2, 0.19, 0.86, 0.22, 10.24, 0.2, 0.22, 0.14, 0.26, 0.25, 0.26, 0.24, 0.24, 0.27, 0.24, 0.47, 0.24, 0.23, 0.25, 0.24, 0.19, 0.22, 0.22, 0.21, 0.21, 0.22, 0.88, 0.21, 10.79, 0.2, 0.21, 0.21, 0.2, 0.21, 0.22, 0.21, 0.2, 0.2, 0.21, 0.2, 0.2, 0.2, 0.21, 0.22, 0.21, 0.24, 0.21, 0.22, 0.2, 0.2, 0.86, 0.22, 10.34, 0.21, 0.75, 0.21, 0.18, 0.16, 0.19, 0.17, 0.16, 0.23, 0.16, 0.16, 0.16, 0.16, 0.18, 0.17, 0.16, 0.16, 0.17, 0.17, 0.16, 0.16, 0.8, 0.16, 10.53, 0.18, 0.17, 0.16, 0.16, 0.16, 0.17, 0.16, 0.16, 0.16, 0.17, 0.17, 0.15, 0.15, 0.17, 0.16, 0.17, 0.15, 0.16, 0.15, 0.17, 0.16, 0.79, 0.15, 11.73, 0.16, 0.16, 0.18, 0.16, 0.16, 0.17, 0.16, 0.19, 0.18, 0.17, 0.16, 0.17, 0.15, 0.17, 0.16, 0.16, 0.15, 0.17, 0.16, 0.18, 0.16, 0.82, 0.16, 10.98, 0.79, 0.18, 0.2, 0.2, 0.14, 0.14, 0.15, 0.2, 0.2, 0.21, 0.2, 0.2, 0.21, 0.31, 0.19, 0.21, 0.2, 0.22, 0.19, 0.22, 0.2, 0.87, 0.2, 10.4, 0.18, 1.49, 0.2, 0.2, 0.2, 0.21, 0.19, 0.19, 0.21, 0.21, 0.22, 0.2, 0.2, 0.21, 0.21, 0.19, 0.2, 0.19, 0.22, 0.19, 0.21, 0.94, 0.2, 12.29, 0.22, 0.21, 0.2, 0.2, 0.19, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.21, 0.21, 0.19, 0.2, 0.2, 0.45, 0.21, 0.2, 0.21, 0.92, 0.22, 9.77, 0.19, 0.25, 0.2, 0.2, 0.19, 0.22, 0.2, 0.21, 0.23, 0.21, 0.19, 0.22, 0.2, 0.2, 0.2, 0.2, 0.2, 0.34, 0.19, 0.21, 0.21, 1.07, 0.18, 10.7, 0.19, 0.24, 0.19, 0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 0.21, 0.18, 0.19, 0.21, 0.22, 0.19, 0.21, 0.21, 0.21, 0.19, 0.2, 0.19, 0.91, 0.2]
lag = 2
test_maxscore=0
train_maxscore = 0
C_max=0
eps_max=0
lag_max = 0
count2predict = 48
y_train = data[:-count2predict]
best_pred = []
#prepare train data
for lag  in range(4,6):
	y_test = data[-(count2predict+lag):]
	X_train = timeSeries_to_supervised(y_train, lag)[:,:-1]
	X_test = timeSeries_to_supervised(y_test,lag)[:,:-1]

	for C_one in range(9,10,1):
		for eps_one in range(15,17):

			print("----------------------------------")
			print("C=",C_one,",epsilon=",eps_one*0.01,",lag=",lag)
			#model and fit
			#clf = SVR(C=110, epsilon=0.06, kernel='rbf')
			clf = SVR(C=C_one, epsilon=eps_one*0.01, kernel='rbf')
			clf.fit(X_train, y_train) 
			#predict train data
			y_pred = clf.predict(X_train)
			#y_pred
			train_one = metrics.r2_score(y_train,y_pred)
			print("R2 score: %f" % train_one)	

			#predict test data
			y_test_pred = clf.predict(X_test)
			#y_test_pred
			if(test_maxscore < metrics.r2_score(y_test,y_test_pred)):
				test_maxscore = metrics.r2_score(y_test,y_test_pred)
				C_max = C_one
				eps_max = eps_one * 0.01
				train_maxscore = train_one
				lag_max = lag
				best_pred = y_test_pred
			print("R2 score: %f" % metrics.r2_score(y_test,y_test_pred))
print("-------------------------------")
print("train_maxscore",train_maxscore)
print("test_maxscore",test_maxscore)
print("C_max",C_max)
print("eps_max",eps_max)
print("lag_max",lag_max)
print("best_pred",best_pred)

plt.plot(range(count2predict),data[-count2predict:],"b+")
plt.plot(range(count2predict),best_pred[-count2predict:],"r+")
plt.show()
