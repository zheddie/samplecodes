from sklearn.svm import SVC
from sklearn.multiclass import OneVsRestClassifier
from sklearn.preprocessing import LabelBinarizer
from sklearn.preprocessing import MultiLabelBinarizer

X = [[1, 2], [2, 4], [4, 5], [3, 2], [3, 1]]
print("X:"+str(X))
y = [0, 0, 1, 1, 2]
print("y:"+str(y))
classif = OneVsRestClassifier(estimator=SVC(random_state=0))
print("predict(X):"+str(classif.fit(X, y).predict(X)))
print("predict([[1,1],[4,1],[4,4]]):"+str(classif.fit(X, y).predict([[1,1],[4,1],[4,4]])))

t = LabelBinarizer().fit_transform(y)

print("t:"+str(t))
print("predict(X):"+str(classif.fit(X, t).predict(X)))
print("predict([[1,1],[4,1],[4,4]]):"+str(classif.fit(X, t).predict([[1,1],[4,1],[4,4]])))

y=[[0,1],[0,2],[1,3],[0,2,4],[2,4]]
z = MultiLabelBinarizer().fit_transform(y)

print("z:"+str(z))
print("predict(X):"+str(classif.fit(X, z).predict(X)))
print("predict([[1,1],[4,1],[4,4]]):"+str(classif.fit(X, z).predict([[1,1],[4,1],[4,4]])))
