#import requests
from lxml import etree
import csv
import time
import asyncio
import aiohttp

from concurrent.futures import ThreadPoolExecutor
f = open("qpz1.data/xinfadi10k.csv","w")
csvwriter = csv.writer(f)

async def download_one_page(url):
    now = time.time()
    headers = {
        "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36"
    }
    #s=aiohttp.ClientSession() <<==>> requests
    # resp = requests.get(url,headers=headers)
    async with aiohttp.ClientSession() as session:
        async with session.get(url,headers=headers) as resp:
            to = time.time()
            if(to - now > 20):
                print("TimeUsed:", (to - now))
                time.sleep(60)
            html = etree.HTML(await resp.text())
            table = html.xpath("/html/body/div[2]/div[4]/div[1]/table")[0]
            trs = table.xpath("./tr")[1:]
            for tr in trs:
                txt = tr.xpath("./td/text()")
                # print(txt)
                txt = [ite.replace("\\",".").replace("/","_") for ite in txt]
                #print(txt)
                csvwriter.writerow(txt)
    print(url,"Download DONE!",time.time()-now)
async def main():
    tasks=[]
    #,15211)
    for idx in range(10000,10020):
        t = asyncio.create_task(download_one_page(f"http://www.xinfadi.com.cn/marketanalysis/0/list/{idx}.shtml"))
        tasks.append(t)
    #print(tasks)
    await asyncio.wait(tasks)
if __name__ == '__main__':
    start = time.time()
    asyncio.run(main())
    print("Total:",time.time()-start)
    f.close()