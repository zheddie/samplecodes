# 将鸢尾属植物数据集分解为训练集和测试集
# 随机排列，用于使分解的数据随机分布
import numpy as np
from sklearn import datasets
iris = datasets.load_iris()
iris_X = iris.data
iris_y = iris.target
np.unique(iris_y)

np.random.seed(0)
indices = np.random.permutation(len(iris_X))
iris_X_train = iris_X[indices[:-10]]
iris_y_train = iris_y[indices[:-10]]
iris_X_test  = iris_X[indices[-10:]]
iris_y_test  = iris_y[indices[-10:]]
# 创建和拟合一个最近邻分类器
from sklearn.neighbors import KNeighborsClassifier
knn = KNeighborsClassifier()
knn.fit(iris_X_train, iris_y_train) 

print("knn.predict(test):"+str(knn.predict(iris_X_test)))

print("real value:"+str(iris_y_test))