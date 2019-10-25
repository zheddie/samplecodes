import urllib;
from bs4 import BeautifulSoup;
import nltk;
from nltk.corpus import stopwords;


response = urllib.urlopen('http://php.net/');
html = response.read();
#print (html);
soup  = BeautifulSoup(html,"html5lib");
text  = soup.get_text(strip = True);
#print(text);
tokens = [t for t in text.split()];
#print(tokens);
freq = nltk.FreqDist(tokens);
for key,val in freq.items():
	print(key.encode("utf-8")+":"+str(val));

#freq.plot(20,cumulative=False);
clean_tokens = tokens;  #why tokens[:]

sr = stopwords.words("english");
for token in tokens:
	if token in sr:
		clean_tokens.remove(token);
freq2 = nltk.FreqDist(clean_tokens);
freq2.plot(20,cumulative=False);
