import requests
from lxml import etree
import csv
import time
from concurrent.futures import ThreadPoolExecutor
f = open("qpz1.data/xinfadi10k.csv","w")
csvwriter = csv.writer(f)

def download_one_page(url):
    now = time.time()
    headers = {
        "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36"
    }
    resp = requests.get(url,headers=headers)
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
    # download_one_page(f"http://www.xinfadi.com.cn/marketanalysis/0/list/{idx}.shtml")
    with ThreadPoolExecutor(50) as t:
        for idx in range(10000,15211):#,15211):
            t.submit(download_one_page,f"http://www.xinfadi.com.cn/marketanalysis/0/list/{idx}.shtml")
    print("Total:",time.time()-start)
    f.close()