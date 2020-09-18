#!/usr/bin/env python
# -*- coding: utf-8 -*-
print("山东宝帅电力铁芯计算")
mianji=input("输入截面积：")
zashu=float(input("输入匝数："))
dianya=input("输入电压：")
if dianya=="":
	dianya=400
else:
	dianya=float(dianya)
panduan=1
mj=0
while panduan!=0 and mianji=="":
	kuandu=input("输入片型宽度：")
	if kuandu=="":
		kuandu=0
	else:
		kuandu=float(kuandu)
	if kuandu==0:
		panduan=0
		continue
	houdu=float(input("输入片型厚度："))
	x=kuandu*houdu
	x=x/100
	mj=mj+x
if mianji!="":
	mj=float(mianji)
cimi=dianya/1.732*45/mj/zashu
print("磁密：",cimi)
xishu=float(input("输入系数："))
zhongliang=float(input("输入重量："))
while True:
	sunhao=float(input("输入单位损耗："))
	p=sunhao*zhongliang*xishu
	print("p0：",p)
	hege=input("p0是否合格？Y（合格）／N（不合格）：")
	if hege=="Y":
		print("计算完毕\n感谢使用！")
		break