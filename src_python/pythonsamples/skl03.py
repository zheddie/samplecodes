from sklearn import svm
from sklearn import datasets
from sklearn.externals import joblib
clf = svm.SVC()
iris = datasets.load_iris()
X,y = iris.data,iris.target
clf.fit(X[:-1],y[:-1])
print(clf.predict(X[-1:]))
joblib.dump(clf,'svcclassify.pkl')
clf2 = joblib.load('svcclassify.pkl')
print ("predict by clf2:")
print(clf2.predict(X[-1:]))
