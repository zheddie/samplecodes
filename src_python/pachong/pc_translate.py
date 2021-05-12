import requests
# url = "https://fanyi.baidu.com/sug"
# fdata = {
#     "kw": "cat"
# }
# resp = requests.post(url,data=fdata)
# print(resp.json())
##############################
# url = "http://dict.cn/picture"
# resp = requests.post(url)
# print(resp.text)

##############################
# url = "http://fanyi.youdao.com/"
url = "http://dict.youdao.com/search"
params = {
    "q":"pickle",
    "keyfrom":"new-fanyi.smartResult"
}
resp = requests.get(url,params=params)
print(resp.text)