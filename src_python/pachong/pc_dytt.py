import requests
import re
import csv

url = "https://www.dy2018.com/"
resp = requests.get(url,verify=False)
resp.encoding = 'gb2312'
print(resp.text)

obj1 = re.compile(r'',re.S)
