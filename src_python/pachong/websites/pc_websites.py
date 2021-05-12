import requests
import re
url = "https://www.sina.com.cn/"
obj = re.compile(r'href=[\'\"](?P<link>.*?)[\"\']')
headers = {
    "user-agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.114 Safari/537.36"
}
resp  = requests.get(url, headers=headers)
rslt = obj.findall(resp.text)
rslt_filter_set = set()
for one in rslt:
    if one != '' and not one.startswith("javascript:"):
        if not one.startswith("http"):
            one = "http:"+one
        rslt_filter_set.add(one)
for one in rslt_filter_set:
    print(one)
print(len(rslt_filter_set))
#print(rslt)
# print(len(rslt))
# rslt_set = set(rslt)
# print(rslt_set,len(rslt_set))
