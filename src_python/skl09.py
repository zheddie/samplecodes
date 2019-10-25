from sklearn import linear_model
from sklearn import datasets
import numpy as np
diabetes = datasets.load_diabetes()
diabetes_X_train = diabetes.data[:-20]
diabetes_X_test  = diabetes.data[-20:]
diabetes_y_train = diabetes.target[:-20]
diabetes_y_test  = diabetes.target[-20:]
regr = linear_model.LinearRegression()
regr.fit(diabetes_X_train, diabetes_y_train)

print("regr.coef:"+str(regr.coef_))



# 均方误差
print("regr.predict(diabetes_X_test):"+str(regr.predict(diabetes_X_test)))
pmean = np.mean((regr.predict(diabetes_X_test)-diabetes_y_test)**2)


# 方差分数：1 是完美的预测
# 0 意味着 X 和 y 之间没有线性关系。
dscore = regr.score(diabetes_X_test, diabetes_y_test) 
print ("pmean:"+str(pmean)+",dscore:"+str(dscore))