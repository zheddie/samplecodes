import numpy as np
from pandas import DataFrame
from pandas import concat
from sklearn import metrics
from sklearn.svm import SVR
# transform time series data to supervised learning data
def timeSeries_to_supervised(data, lag=1):
    df = DataFrame(data)
    columns = [df.shift(i) for i in range(1,lag+1)]
    columns.append(df)
    df = concat(columns, axis=1)
    df.fillna((data[0]+data[1])/2, inplace=True)
    return df.values

#all the data
data = [17.61,18.1,15.21,16.4,19.02,10.39,18.88,32.37,10.77,10.55,8.78,10.37,24.23,13.91,9.03,10.09,
10.09,10.37,9.92,11.22,10.12,10.89,10.54,10.37,10.34,10.9,10.17,9.94,23.89,9.55,23.89,22.89,8.69,
25.11,9.4,26.59,9.74,24.89,9.8,24.25,9.95,8.55,13.94,11.82,10.24,9.95,21.7,26.2,22.01,25.66,31.87,
10.62,10.83,10.94,10.94,12.74,10.36,10.66,11.01,10.5,10.18,11.95,25.6,10.67,9.78,10.45,10.38,
10.86,24.36,22.94,10.59,10.32,9.37,10.18,9.5]
#prepare train data
y_train = data[:-9]
X_train = timeSeries_to_supervised(y_train, 2)[:,:-1]
#model and fit
clf = SVR(C=110, epsilon=0.06, kernel='rbf')
clf.fit(X_train, y_train) 
#predict train data
y_pred = clf.predict(X_train)
y_pred
print("R2 score: %f" % metrics.r2_score(y_train,y_pred))	

#prepare test data
y_test = data[-9:]
X1_test = data[-10:-1]
X2_test = data[-11:-2]
X1_test = np.array(X1_test).reshape(-1,1);
X2_test = np.array(X2_test).reshape(-1,1);
X_test = np.hstack((X1_test,X2_test));
#predict test data
y_test_pred = clf.predict(X_test)
y_test_pred
print("R2 score: %f" % metrics.r2_score(y_test,y_test_pred))


