import requests
import re
import os

def dl_zherenwanghou_m3u8():
    obj = re.compile(r"url: '(?P<m3u8uri>.*?)',",re.S)
    headers = {
        "user-agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.114 Safari/537.36"
    }
    url = "https://www.91kanju.com/vod-play/56169-1-1.html"
    resp = requests.get(url,headers=headers)
    m3u8_url = obj.search(resp.text).group("m3u8uri")
    resp.close()

    print(m3u8_url)
    resp2 = requests.get(m3u8_url,headers=headers)

    with open('qpz1.data/zherenwanghou.m3u8','wb') as f:
        f.write(resp2.content)
    resp2.close()
    print("DONE!")

def dl_zherenwanghou():
    idx=1
    with open("qpz1.data/zherenwanghou.m3u8",'r',encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line.startswith("#"):
                continue
            resp3 = requests.get(line)
            f = open(f"data/zrwh_{idx}.ts","wb")
            f.write(resp3.content)
            f.close()
            resp3.close()
            print(f"idx:{idx}")
            idx+=1
def dl_zixiushi_m3u8():
    url = "https://zkcdn.wb699.com/2019/01/12/yi0g2fYjW4yx4zax/playlist.m3u8"
    resp = requests.get(url)
    with open("qpz1.data/zixiushi.m3u8","wb") as f:
        f.write(resp.content)
    resp.close()
def dl_zixiushi():
    headers = {
        "user-agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.114 Safari/537.36"
    }
    f = open("qpz1.data/zixiushi.m3u8","r",encoding="utf-8")
    idx = 1
    for line in f:
        if line.startswith("#"):
            continue
        line=line.strip()
        url = f"https://zkcdn.wb699.com/2019/01/12/yi0g2fYjW4yx4zax/{line}"
        print(f"{url}")
        resp = requests.get(url,headers=headers,verify=False)
        filename = f"qpz1.data/zxs_{line}"
        if not os.path.exists(filename):
            c = open(filename,"wb")
            c.write(resp.content)
            c.close()
        resp.close()
        print(f"DONE:{url}")
        idx+=1
if __name__ == '__main__':
    #dl_zixiushi_m3u8()
    dl_zixiushi()