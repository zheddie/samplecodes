from sklearn import tree
# 1. collect tranining data
# input: [weight, 1-> smooth | 0 -> bumpy]
features = [[140, 1], [130, 1], [150, 0], [170, 0]]
# output: apple -> 0 | orange -> 1
labels = [0, 0, 1, 1]
# 2. train claasifier
clf = tree.DecisionTreeClassifier()
clf = clf.fit(features, labels)
# 3. make predict
# 150kg, bumpy
print (clf.predict([[150, 0]]))
