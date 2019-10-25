from sklearn import tree
features = [[140,1],[142,0],[149,1],[152,1],[150,1],[148,1],[130,0],[151,0]]
labels = [1,1,1,1,1,1,0,0]
clf = tree.DecisionTreeClassifier()
clf.fit(features,labels)
print clf.predict([[149,0]])

