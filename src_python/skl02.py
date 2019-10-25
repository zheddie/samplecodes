from sklearn import svm
from sklearn import datasets
import pickle
clf = svm.SVC()
iris = datasets.load_iris()
X,y = iris.data,iris.target
clf.fit(X[:-1],y[:-1])
print(clf.predict(X[-1:]))
s = pickle.dumps(iris)
iris = pickle.loads(s)
print(iris.data.shape)
