import kNN

grps , lbls = kNN.createDataSet();
#rate = kNN.classify0([1,0.8],grps,lbls,3);
rate = kNN.classify0([2,0.8],grps,lbls,3);
print (rate)
