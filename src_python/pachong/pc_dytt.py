# -*- coding: UTF-8 -*-
import requests
import re
import csv

def getContent(url):
    headers = {
        "User-Agent":"Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:87.0) Gecko/20100101 Firefox/87.0"
    }
    resp = requests.get(url,verify=False,headers = headers)
    resp.encoding = 'gb2312'
    return(resp)


domain = "https://www.dy2018.com/"
# resp = requests.get(url,verify=False)
# resp.encoding = 'gb2312'
resp = getContent(domain)

#print(resp.text)

obj1 = re.compile(r'2020必看热片.*?<ul>(?P<ul>.*?)</ul>',re.S)
obj2 = re.compile (r"<a href='(?P<url>.*?)'.*?>(?P<name>.*?)</a>",re.S)
obj3 = re.compile(r'◎片　　名(?P<pianming>.*?)<br.*?◎年　　代(?P<year>.*?)<br.*?<td style="WORD-WRAP: break-word" bgcolor="#fdfddf"><a href="(?P<downloadurl>.*?)">',re.S)
iter = obj1.finditer(resp.text)

for it  in iter:
    #print(it.group('ul'))
    iter2 = obj2.finditer(it.group('ul'))
    for it2 in iter2:
        print(it2.groupdict())
resp.close()
url = 'https://www.dy2018.com//i/102919.html'
onefile = getContent(url)
print(onefile.text)
iter3  = obj3.finditer(onefile.text)
for it3 in iter3:
    print(it3.groupdict()) 
onefile.close()
