import requests
from lxml import etree
import csv
import time
from concurrent.futures import ThreadPoolExecutor
f = open("qpz1.data/xinfadi.extra.csv","w")
csvwriter = csv.writer(f)

def download_one_page(url):
    now = time.time()
    headers = {
        "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.0.3 Safari/605.1.15"
        ,"Connection": "keep-alive"
    }
    #proxies = {"http":"http://106.53.38.69:3128"}
    #print("URL:",url)
    try:
        resp = requests.get(url,headers=headers)#,proxies=proxies)
    except requests.exceptions.ConnectionError:
        print("ERROR:Connection refused")
        return()
    #print("resp:",resp)
    to = time.time()
    if(to - now > 20):
        print("TimeUsed:", (to - now))
        time.sleep(60)
    html = etree.HTML(resp.text)
    
    table = html.xpath("/html/body/div[2]/div[4]/div[1]/table")[0]
    trs = table.xpath("./tr")[1:]
    for tr in trs:
        txt = tr.xpath("./td/text()")
        # print(txt)
        txt = [ite.replace("\\",".").replace("/","_") for ite in txt]
        #print(txt)
        csvwriter.writerow(txt)
    resp.close()
    print(url,"Download DONE!",time.time()-now)
if __name__ == '__main__':
    start = time.time()
    # idx = 5555
    # download_one_page(f"http://118.190.9.138/marketanalysis/0/list/{idx}.shtml")
    with ThreadPoolExecutor(50) as t:
        for idx in (15224,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15):#,15211,15224):
            t.submit(download_one_page,f"http://www.xinfadi.com.cn/marketanalysis/0/list/{idx}.shtml")
    print("Total:",time.time()-start)
    f.close()