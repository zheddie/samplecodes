# Download MeiLiXinShiJie from 91kanju.com
# 1) https://www.91kanju.com/vod-play/57316-1-1.html
#   m3u8 
#   video: {
#        url: 'https://m3api.awenhao.com/index.php?note=kkRrxsh521bm9tgke6cay&raw=1&n.m3u8',
#	type: 'customHls',
# 2) downloads ts
# 3) AES-
# 4) merge.
import requests
import re
from pc_utils import *
import asyncio
import aiohttp
import aiofiles
import os

def get_mlxsj_m3u8_url():
    url = "https://www.91kanju.com/vod-play/57316-1-1.html"
    resp = requests.get(url)
    obj = re.compile(r"url: '(?P<m3u8_url>.*?)',",re.S)
    m3u8_url = obj.search(resp.text).group("m3u8_url")
    resp.close()
    return(m3u8_url)
    
async def aio_dl_tss(m3u8name,headers={}):
    ts_path = m3u8name.split("/")[0]
    tasks=[]
    async with aiohttp.ClientSession() as session:
        async with aiofiles.open(m3u8name,"r",encoding='utf-8') as f:
            async for line in f:
                if not line.startswith("#"):
                    line = line.strip()
                    name = line.split("/")[-1].split("?")[0]
                    name = f"{ts_path}/{name}"
                    task = asyncio.create_task(aio_dl_ts(session,line,name,headers))
                    tasks.append(task)
            await asyncio.wait(tasks)

async def aio_dec_tss(m3u8name,key=b''):
    ts_path = m3u8name.split("/")[0]
    tasks=[]
    async with aiofiles.open(m3u8name,"r",encoding='utf-8') as f:
        async for line in f:
            if not line.startswith("#"):
                line = line.strip()
                name = line.split("/")[-1].split("?")[0]
                name = f"{ts_path}/{name}"
                task = asyncio.create_task(aio_dec_ts(name,f"{name}.ts",key))
                tasks.append(task)
        await asyncio.wait(tasks)

def dl_key_file(m3u8name,headers={}):
    ts_path = m3u8name.split("/")[0]
    key_url = get_match(m3u8name,'METHOD=AES-128,URI="','"')
    resp = requests.get(key_url,headers=headers)
    key_file = ts_path+"/"+key_url.split('/')[-1]
    with open(key_file,'wb') as f:
        f.write(resp.content)
    return(resp.content)

def merge_film(m3u8name,targetname):
    cmd = "cat"
    ts_path = m3u8name.split("/")[0]
    with open(m3u8name,"r",encoding='utf-8') as f:
        for line in f:
            if not line.startswith("#"):
                line = line.strip()
                name = line.split("/")[-1].split("?")[0]
                name = f"{ts_path}/{name}.ts"
                cmd+= f" {name}"
    cmd += f"> {ts_path}/allinall.ts"
    #print(cmd)
    os.system(cmd)
    ffmpegcmd=f"ffmpeg -i allinall.ts -acodec copy -vcodec copy -bsf:a aac_adtstoasc {targetname}"
    print(ffmpegcmd)
    os.system(ffmpegcmd)

def main():
#     # #1
#     # m3u8_url = get_mlxsj_m3u8_url()
#     # #m3u8_url = "https://m3api.awenhao.com/index.php?note=kkR3kw6n91drzj7hfcx5b&raw=1&n.m3u8"
#     # #1.2
#     headers={
#         # "origin": "https://www.91kanju.com",
#         # Since"user-agent": "'Custom'"
#         "user-agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.114 Safari/537.36"
#     }

#     # #m3u8_url = "https://m3api.awenhao.com/index.php?"
    m3u8_filename="qpz1.mlxsj/mlxsj.m3u8"
#     # dl_url_to_file(m3u8_url,m3u8_filename,headers)
#     #2 download tss
# #    asyncio.run(aio_dl_tss(m3u8_filename,headers))
#     #2.1 get key file
#     key = dl_key_file(m3u8_filename,headers)
#     #3 AES
#     asyncio.run(aio_dec_tss(m3u8_filename,key))
    #4 merge it.
    merge_film(m3u8_filename,"mlxsj.mp4")

if __name__ == '__main__':
    main()
    print("main DONE.")