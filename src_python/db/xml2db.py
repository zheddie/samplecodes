#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# This code is supporting material for the book
# Building Machine Learning Systems with Python
# by Willi Richert and Luis Pedro Coelho
# published by PACKT Publishing
#
# It is made available under the MIT License

#
# This script filters the posts and keeps those posts that are or belong
# to a question that has been asked in 2011 or 2012.
#

import sys
import os
import re
import ibm_db_dbi as dbi

from dateutil import parser as dateparser

from operator import itemgetter
from xml.etree import cElementTree as etree
from collections import defaultdict


DATA_DIR = "data"

filename = os.path.join(DATA_DIR, "Posts.xml")
print("Reading from xml %s" % filename)
databasename = "qmldemo"
tname_filtered = "fltr"
tname_filtered_meta = "fltrmeta"

q_creation = {}  # creation datetimes of questions
q_accepted = {}  # id of accepted answer

# question -> [(answer Id, IsAccepted, TimeToAnswer, Score), ...]
meta = defaultdict(list)

# regegx to find code snippets
code_match = re.compile('<pre>(.*?)</pre>|<code>(.*?)</code>', re.MULTILINE | re.DOTALL)
#code_match = re.compile('<code>(.*?)</code>', re.MULTILINE | re.DOTALL)
link_match = re.compile(
    '<a href="http://.*?".*?>(.*?)</a>', re.MULTILINE | re.DOTALL)
img_match = re.compile('<img(.*?)/>', re.MULTILINE | re.DOTALL)
tag_match = re.compile('<[^>]*>', re.MULTILINE | re.DOTALL)


def filter_html(s):
    num_code_lines = 0
    link_count_in_code = 0
    code_free_s = s

    num_images = len(img_match.findall(s))

    # remove source code and count how many lines
    for match_str in code_match.findall(s):
        match_str = ''.join(match_str)    # we need to join it, as if we use | in patten, we would get a list as each item in findall. The list would have '' for the one which does not match anything.  For example: "a(b)c|d(e)f" would return something like: [('b',''),('','e')]
        num_code_lines += match_str.count('\n')
        code_free_s = code_match.sub("", code_free_s)

        # sometimes source code contain links, which we don't want to count
        link_count_in_code += len(link_match.findall(match_str))

    links = link_match.findall(s)
    link_count = len(links)

    link_count -= link_count_in_code

    link_free_s = re.sub(
        " +", " ", tag_match.sub('', code_free_s)).replace("\n", "")

    for link in links:
        if link.lower().startswith("http://"):
            link_free_s = link_free_s.replace(link, '')

    num_text_tokens = link_free_s.count(" ")

    return link_free_s, num_text_tokens, num_code_lines, link_count, num_images

years = defaultdict(int)
num_questions = 0
num_answers = 0

if sys.version_info.major < 3:
    # Python 2, map() returns a list, which will lead to out of memory errors.
    # The following import ensures that the script behaves like being executed
    # with Python 3.
    from itertools import imap as map


def parsexml(filename):
    global num_questions, num_answers

    counter = 0

    it = map(itemgetter(1),
             iter(etree.iterparse(filename, events=('start',))))

    root = next(it)  # get posts element

    for elem in it:
        if counter % 100000 == 0:
            print("Processed %i <row/> elements" % counter)
        #print("zg.Processed %i <row/> elements" % counter)
        counter += 1

        if elem.tag == 'row':
            creation_date = dateparser.parse(elem.get('CreationDate'))

            Id = int(elem.get('Id'))
            PostTypeId = int(elem.get('PostTypeId'))
            Score = int(elem.get('Score'))

            if PostTypeId == 1:
                num_questions += 1
                years[creation_date.year] += 1

                ParentId = -1
                TimeToAnswer = 0
                q_creation[Id] = creation_date
                accepted = elem.get('AcceptedAnswerId')
                if accepted:
                    q_accepted[Id] = int(accepted)
                IsAccepted = 0

            elif PostTypeId == 2:
                num_answers += 1

                ParentId = int(elem.get('ParentId'))
                if not ParentId in q_creation:
                    # question was too far in the past
                    continue

                TimeToAnswer = (creation_date - q_creation[ParentId]).seconds

                if ParentId in q_accepted:
                    IsAccepted = int(q_accepted[ParentId] == Id)
                else:
                    IsAccepted = 0

                meta[ParentId].append((Id, IsAccepted, TimeToAnswer, Score))

            else:
                continue

            Text, NumTextTokens, NumCodeLines, LinkCount, NumImages = filter_html(
                elem.get('Body'))

            values = [Id, ParentId,
                      IsAccepted,
                      TimeToAnswer, Score,
                      Text.encode("utf-8"),
                      NumTextTokens, NumCodeLines, LinkCount, NumImages]

            yield values

            root.clear()  # preserve memory
def writetodb(tbname,values,strlist=[],record=0):
    valuesstr = ""
    idx = 0
    global cur
    for oneval in values:
        if idx in strlist:
            oneval = '\''+oneval+'\''
        else:
            oneval = str(oneval)
        valuesstr+= oneval+','
        idx +=1
    valuesstr = valuesstr[:-1]
    sqlcmd = "insert into "+tbname+" values("+valuesstr+")"
    if record % 10000 == 0:
        print(record,sqlcmd)
    try:
        cur.execute(sqlcmd)
    except Exception as err:
        print('ERROR:  ' + str(err))
        print(record,sqlcmd)

conn = dbi.connect()
conn.charset="utf8"
cur = conn.cursor()
reccnt = 0

for values in parsexml(filename):
    values[5] = str(values[5])[2:-1].replace('\\','\\\\').replace('\'','\'\'')
    #line = "\t".join(map(str, values))
    writetodb(databasename+"."+tname_filtered,values,strlist=[5],record=reccnt)
    reccnt +=1
conn.commit()
# for values in parsexml(filename):
#     continue
reccnt = 0
for onekey in meta:
    for oneans in meta[onekey]:
        values = [onekey]
        values.extend(list(oneans))
        writetodb(databasename+"."+tname_filtered_meta,values,record=reccnt)
    reccnt += 1
conn.commit()
cur.close()
conn.close()
print("years:", years)
print("#qestions: %i" % num_questions)
print("#answers: %i" % num_answers)
