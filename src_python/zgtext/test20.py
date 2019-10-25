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
twenty_train = fetch_20newsgroups(subset='train',shuffle=True,random_state=42)
idx = 0
for name in twenty_train.target_names:
	print("name"+str(idx)+":"+name)
	idx+=1
# text_clf = Pipeline([('vect', CountVectorizer()),
#                      ('tfidf', TfidfTransformer()),
#                      ('clf', SGDClassifier(loss='hinge', penalty='l2',
#                                            alpha=1e-3, random_state=42,
#                                            max_iter=5, tol=None)),
# ])

parameters = {'vect__ngram_range': [(1, 1), (1, 2)],
              'tfidf__use_idf': (True, False),
              'clf__alpha': (1e-2, 1e-3),
}
# gs_clf = GridSearchCV(text_clf, parameters, n_jobs=-1)
# #gs_clf = gs_clf.fit(twenty_train.data[:400], twenty_train.target[:400])
# gs_clf = gs_clf.fit(twenty_train.data, twenty_train.target)
gs_clf = cu.readModule("20classifier.pkl")
print("gs_clf.best_score_:"+str(gs_clf.best_score_))
print(str(twenty_train.target_names[gs_clf.predict(['God is love'])[0]]))

pv = gs_clf.predict(['screen is in black.']);
print("pv:"+str(pv))
print(str(twenty_train.target_names[pv[0]]))

for param_name in sorted(parameters.keys()):
	print("%s: %r" % (param_name, gs_clf.best_params_[param_name]))

#zg.evaluation for pipeline
# import numpy as np
twenty_test = fetch_20newsgroups(subset='test', shuffle=True, random_state=42)
docs_test = twenty_test.data
predicted = gs_clf.predict(docs_test)
pmean = np.mean(predicted == twenty_test.target)  
print("pmean:"+str(pmean))


    