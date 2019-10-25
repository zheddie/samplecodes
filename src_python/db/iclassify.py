#!/usr/bin/env python3
# This code is supporting material for the book
# Building Machine Learning Systems with Python
# by Willi Richert and Luis Pedro Coelho
# published by PACKT Publishing
#
# It is made available under the MIT License

import time
start_time = time.time()

import numpy as np
import nltk
from sklearn.metrics import classification_report
from sklearn.metrics import precision_recall_curve, roc_curve, auc
from sklearn.model_selection import KFold
from sklearn import neighbors
from sklearn.linear_model import LogisticRegression

import ibm_db_dbi as dbi

all_questions = set() #this variable is not used. 
all_answers = []
def runsql(sqlcmd):
    global cur
    rlist = []
    try:
        cur.execute(sqlcmd)
        if cur._result_set_produced:
            rlist = cur.fetchall()
    except Exception as err:
        print('ERROR:  ' + str(err))
    return rlist

conn = dbi.connect()
conn.charset="utf8"
cur = conn.cursor()

sqlcmd = "select a.ppid,a.pid,a.score \
   from qmldemo.fltrmeta as a, \
        (select x.ppid,x.xs as bxs,y.ys as bys \
        from (select ppid,max(score) as xs from qmldemo.fltrmeta  group by ppid having max(score) >0 order by ppid) as x, \
        (select ppid,min(score) as ys from qmldemo.fltrmeta group by ppid having min(score)<0 order by ppid) as y \
        where x.ppid=y.ppid order by x.ppid) as b \
    where a.ppid= b.ppid and (a.score = b.bxs or a.score = b.bys) order by a.ppid"
qryout = runsql(sqlcmd)
print("Records from db:"+str(len(qryout)))
p_ppid = -2 #make sure it's not something like -1, which means it's an answer,although it would not happen in this result.
rlimit = [1,1]

for rec in qryout:
    ppid, pid,score = rec
    if p_ppid != ppid:
        rlimit=[1,1]
        p_ppid = ppid
    if score >0 and rlimit[1]!= 0:
        all_questions.add(ppid)
        all_answers.append(pid)
        rlimit[1] = 0
    if score < 0 and rlimit[0] != 0:
        all_questions.add(ppid)
        all_answers.append(pid) 
        rlimit[0] =0  
    #print(rec)
print(len(all_answers),len(all_questions))
#print("time spent0:", time.time() - start_time)
classifying_answer = "poor"

sqlcmd = "select pid,ppid,txt,score,ntkns,ncdl,nlnkcnt,nimgs \
        from qmldemo.fltr \
        where pid in "+str(tuple(all_answers))
#print(sqlcmd)

qryout = runsql(sqlcmd)
print("Time passed after the db retrive:", time.time() - start_time)
# qa_X = np.array([],type=float)
# qa_Y = np.array([],type=float)
ql_X = []
ql_Y = []
for rec in qryout:
    pid,ppid,text,score,ntkns,ncdl,nlnkcnt,nimgs = rec
    if text == '':
        avgsl = 0
        avgwl = 0
    else:
        #text = text.decode('utf-8')
        text = text.replace('\\\\','\\')
        text = text.replace('\\x..','')
        text = text.replace('\\','')
        sent_lens = [len(nltk.word_tokenize(
        sent)) for sent in nltk.sent_tokenize(text)]

        avgsl = np.mean(sent_lens)
        avgwl = np.mean([len(w) for w in nltk.word_tokenize(text)])
    nallcaps = np.sum([word.isupper() for word in nltk.word_tokenize(text)])
    nexclams = text.count('!')
    ql_X.append([ntkns,ncdl,nlnkcnt,avgsl,avgwl,nallcaps,nexclams,nimgs])
    if classifying_answer == "good":
        ql_Y.append(score > 0)
    elif classifying_answer == "poor":
        ql_Y.append(score < 0)
#print(ql_X,ql_Y)
qa_X = np.asarray(ql_X,dtype=float)
qa_Y = np.asarray(ql_Y,dtype = bool)
np.set_printoptions(threshold=np.nan)
#print(qa_X,qa_Y)


avg_scores_summary = []

print("time passed after the features data ready:", time.time() - start_time)
def measure(clf_class, parameters, name, data_size=None):
    start_time_clf = time.time()
    #print(type(qa_X),qa_X.shape,type(qa_Y),qa_Y.shape)
    if data_size is None:
        X = qa_X
        Y = qa_Y
    else:
        X = qa_X[:data_size]
        Y = qa_Y[:data_size]
    real_data_size = len(X);
    kf = KFold(n_splits=3)

    train_errors = []
    test_errors = []

    scores = []
    roc_scores = []
    fprs, tprs = [], []

    pr_scores = []
    precisions, recalls, thresholds = [], [], []
    fold_idx = 0
    for train, test in kf.split(X):
        X_train, y_train = X[train], Y[train]
        X_test, y_test = X[test], Y[test]

        only_one_class_in_train = len(set(y_train)) == 1
        only_one_class_in_test = len(set(y_test)) == 1
        if only_one_class_in_train or only_one_class_in_test:
            # this would pose problems later on
            continue

        clf = clf_class(**parameters)

        clf.fit(X_train, y_train)

        train_score = clf.score(X_train, y_train)
        #print(X_test, y_test)
        test_score = clf.score(X_test, y_test)

        train_errors.append(1 - train_score)
        test_errors.append(1 - test_score)

        scores.append(test_score)
        proba = clf.predict_proba(X_test)
        
        label_idx = 1
        #print(y_test.shape,proba.shape)
        fpr, tpr, roc_thresholds = roc_curve(y_test, proba[:, label_idx])
        precision, recall, pr_thresholds = precision_recall_curve(
            y_test, proba[:, label_idx])

        roc_scores.append(auc(fpr, tpr))
        fprs.append(fpr)
        tprs.append(tpr)

        pr_scores.append(auc(recall, precision))
        precisions.append(precision)
        recalls.append(recall)
        thresholds.append(pr_thresholds)

        # This threshold is determined at the end of the chapter 5,
        # where we find conditions such that precision is in the area of
        # about 80%. With it we trade off recall for precision.
        threshold_for_detecting_good_answers = 0.63

        print("Split #%i , trainsize:%i,testsize=%i" % (fold_idx,len(X_train),len(X_test)))
        print(classification_report(y_test, proba[:, label_idx] >
              threshold_for_detecting_good_answers, target_names=['not accepted', 'accepted']))
        fold_idx +=1
    # get medium clone
    scores_to_sort = pr_scores  # roc_scores
    medium = np.argsort(scores_to_sort)[int(len(scores_to_sort) / 2)]
    #print("Medium clone is #%i" % medium)

    precisions = precisions[medium]
    recalls = recalls[medium]
    thresholds = np.hstack(([0], thresholds[medium]))
    idx80 = precisions >= 0.8
    print("C="+str(parameters['C'])+":P=%.2f R=%.2f thresh=%.2f" % (precisions[idx80][0], recalls[idx80][0], thresholds[idx80][0]))

    return np.mean(train_errors), np.mean(test_errors)

for C in [0.1]:
#for C in [0.01, 0.1, 1.0, 10.0]:
    name = "LG C=%.2f" % C
    tr_err,tst_err = measure(LogisticRegression, {'penalty': 'l2', 'C': C}, name)
    print("Overall:train error:"+str(tr_err)+",test error:"+str(tst_err))
print("Time passed after the evaluation of the model:", time.time() - start_time)
