import requests
import re
import csv
start = 0
f = open('douban.csv','w')
csvw = csv.writer(f)
while(start<250):
    url = f"https://movie.douban.com/top250?start={start}&filter="
    print(url)
    headers = {
        "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36"
    }
    res = requests.get(url,headers=headers)
    #print(res.text)
    pagecontent = res.text
    obj = re.compile(r'<li>.*?<span class="title">(?P<name>.*?)</span>.*?<p class="">'
        r'.*?<br>.*?(?P<year>\d*?)&nbsp;/&nbsp;.*?<span class="rating_num" property="v:average">'
        r'(?P<rate>.*?)</span>.*?<span>(?P<count>.*?)人评价</span>',re.S)
    iter = obj.finditer(pagecontent)
    

    for it in iter:
        dic = it.groupdict()
        csvw.writerow(dic.values())
        #print(dic)
        #print(it.group("name"),it.group("year"),it.group("rate"),it.group("count"))
    res.close()
    start+=25
f.close()