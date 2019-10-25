from sklearn.datasets import fetch_20newsgroups
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.naive_bayes import MultinomialNB
from sklearn.pipeline import Pipeline
from sklearn.linear_model import SGDClassifier
from sklearn.model_selection import GridSearchCV
import numpy as np
import commonuse as cu

categories  = ['alt.atheism', 'soc.religion.christian',
              'comp.graphics', 'sci.med']
#print("before fetch\n")
twenty_train = fetch_20newsgroups(subset='train',categories = categories,shuffle=True,random_state=42)
#print("after fetch\n")
# count_vect = CountVectorizer()
# # twenty_train.target_names
# # len(twenty_train.data)

# # len(twenty_train.filenames)

# # print("\n".join(twenty_train.data[0].split("\n")[:3]))
# # print(twenty_train.target_names[twenty_train.target[0]])
# # for t in twenty_train.target[:10]:
# #     print(twenty_train.target_names[t])

# #zg.feature extract.
# X_train_counts = count_vect.fit_transform(twenty_train.data)
# # X_train_counts.shape
# # count_vect.vocabulary_.get(u'algorithm')
# #
# #zg.transform the data
# #tf_transformer = TfidfTransformer(use_idf=False).fit(X_train_counts)
# #X_train_tf = tf_transformer.transform(X_train_counts)
# #X_train_tf.shape

# tfidf_transformer = TfidfTransformer()
# X_train_tfidf = tfidf_transformer.fit_transform(X_train_counts)
# #X_train_tfidf.shape

# #zg.training the MultinomialNB
# clf = MultinomialNB().fit(X_train_tfidf, twenty_train.target)

# #zg.try to predict
# docs_new = ['God is love', 'OpenGL on the GPU is fast']
# X_new_counts = count_vect.transform(docs_new)
# X_new_tfidf = tfidf_transformer.transform(X_new_counts)

# predicted = clf.predict(X_new_tfidf)

# for doc, category in zip(docs_new, predicted):
#     print('%r => %s' % (doc, twenty_train.target_names[category]))
# #zg.evalution.

# twenty_test = fetch_20newsgroups(subset='test',
#     categories=categories, shuffle=True, random_state=42)
# X_test_counts = count_vect.transform(twenty_test.data)
# X_test_tfidf = tfidf_transformer.fit_transform(X_test_counts)

# predicted = clf.predict(X_test_tfidf)
# pmean = np.mean(predicted == twenty_test.target)  
# print("pmean:"+str(pmean))
#zg.More efficient way. Pipeline:
# from sklearn.pipeline import Pipeline
# text_clf = Pipeline([('vect', CountVectorizer()),
#                      ('tfidf', TfidfTransformer()),
#                      ('clf', MultinomialNB()),
# ])

# text_clf.fit(twenty_train.data, twenty_train.target)  



#zg.Try SVM to imporve performance

text_clf = Pipeline([('vect', CountVectorizer()),
                     ('tfidf', TfidfTransformer()),
                     ('clf', SGDClassifier(loss='hinge', penalty='l2',
                                           alpha=1e-3, random_state=42,
                                           max_iter=5, tol=None)),
])
# text_clf.fit(twenty_train.data, twenty_train.target)  

# predicted = text_clf.predict(docs_test)
# np.mean(predicted == twenty_test.target) 

#zg.Better evaluation way.
# from sklearn import metrics
# print(metrics.classification_report(twenty_test.target, predicted,
#     target_names=twenty_test.target_names))

# metrics.confusion_matrix(twenty_test.target, predicted)

#zg. param auto-refine.
# 
parameters = {'vect__ngram_range': [(1, 1), (1, 2)],
              'tfidf__use_idf': (True, False),
              'clf__alpha': (1e-2, 1e-3),
}
gs_clf = GridSearchCV(text_clf, parameters, n_jobs=-1)
#gs_clf = gs_clf.fit(twenty_train.data[:400], twenty_train.target[:400])
gs_clf = gs_clf.fit(twenty_train.data, twenty_train.target)
print(str(twenty_train.target_names[gs_clf.predict(['God is love'])[0]]))
print(gs_clf.best_score_)
for param_name in sorted(parameters.keys()):
	print("%s: %r" % (param_name, gs_clf.best_params_[param_name]))
cu.saveModule(gs_clf,"4classifier.pkl")
#zg.evaluation for pipeline
# import numpy as np
twenty_test = fetch_20newsgroups(subset='test',
    categories=categories, shuffle=True, random_state=42)
docs_test = twenty_test.data
predicted = gs_clf.predict(docs_test)
pmean = np.mean(predicted == twenty_test.target)  
print("pmean:"+str(pmean))


    