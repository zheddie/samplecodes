import requests
import asyncio
import aiofiles
import re
from Crypto.Cipher import AES
def get_match(filename,pref,suf):
    pat = re.compile(pref+'(?P<target>.*?)'+suf,re.S)
    with open(filename,'r',encoding='utf-8') as f:
        content = f.read()
        return(pat.search(content).group('target'))

def dl_url_to_file(url,name,headers={},params={}):
    #print(url,name)
    resp = requests.get(url,headers=headers,params=params)
    # print(resp.status_code)
    with open(name,'wb') as f:
        f.write(resp.content)
        f.close()
    resp.close()
async def aio_dl_ts(session,url,name,headers={}):
    async with session.get(url,headers=headers) as resp:
        async with aiofiles.open(name,"wb") as f:
            await f.write(await resp.read())
    print(f"{name} done.")
async def aio_dec_ts(srcname,targetname,key=b''):
    aes = AES.new(key=key,IV=b'0000000000000000',mode=AES.MODE_CBC)
    async with aiofiles.open(srcname,'rb') as f1, aiofiles.open(targetname,'wb') as f2:
            bs = await f1.read()
            await f2.write(aes.decrypt(bs))
    print(f"{targetname} done.")

