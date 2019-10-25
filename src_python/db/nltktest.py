#!/usr/bin/env python3

import nltk
import numpy as np
text = "Hello Mark. This is Gavin. We met in the conference last week. I still have some questions on the SLIC code. Can you do some help on this area to me ? Thanks!"
sent_lens = [len(nltk.word_tokenize(sent)) for sent in nltk.sent_tokenize(text)]
avgsl = np.mean(sent_lens)
avgwl = np.mean([len(w) for w in nltk.word_tokenize(text)])
nallcaps = np.sum([word.isupper() for word in nltk.word_tokenize(text)])
nexclams = text.count('!')

print(avgsl,avgwl,nallcaps,nexclams)

