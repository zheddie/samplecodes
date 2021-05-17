#e23.1CrawUnivRanking.py

# -*- coding: utf-8 -*-

import requests
from bs4 import BeautifulSoup
allUniv = []
def getHTMLText(url):
    try:
        r = requests.get(url, timeout=30)
        r.raise_for_status()
        r.encoding = 'utf-8'
        return r.text
    except:
        return ""
def fillUnivList(soup):
    data = soup.find_all('tr')
    for tr in data:
        ltd = tr.find_all('td')
        if len(ltd)==0:
            continue
        singleUniv = []
        for td in ltd:
            singleUniv.append(td.string)
        allUniv.append(singleUniv)
def printUnivList(num):
    print("{:^4}{:^10}{:^5}{:^8}{:^10}".format("排名","学校名称","省市","总分","培养规模"))
    for i in range(num):
        u=allUniv[i]
        print(u)
        #print("{:^4}{:^10}{:^5}{:^8}{:^10}".format(trim(u[0]),u[2],u[3],u[4],u[5]))
def main():
    #url = 'http://www.zuihaodaxue.cn/zuihaodaxuepaiming2016.html'
    url = 'http://www.shanghairanking.cn/rankings/bcur/2020'
    html = getHTMLText(url)
    soup = BeautifulSoup(html, "html.parser")
    fillUnivList(soup)
    printUnivList(10)
main()