from urllib.request import urlopen
url = "http://www.baidu.com"
resp = urlopen(url)
with open("baidu.html","w") as f:
    f.write(resp.read().decode("utf-8"))
print("done!")