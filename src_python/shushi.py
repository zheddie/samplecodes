import sys
def printoutadd(a,b):
	print("\t%10s"%a)
	print("%8s%10s"%("+",b))
	print("---------------------------")
	print("\t%10d"%(int(a)+int(b)))
def getshang(a,b):
	s = 0
	while(a-b >= 0):
		s+=1
		a = a-b
	return(s,a)
def printoutdiv(a,b):
	ia = int(a)
	ib = int(b)
	lenb = len(b)
	lena = len(a)
	res = list(a)
	ir=0
	iy=0
	lji = []
	lyu = []
	for i in range(lena):
		res[i]='0'
	basepos = lenb-1
	if basepos >lena-1:
		shang = 0
		yu = ia
	else:
		x = a[:lenb]
		for i in range(lena-basepos):
			ix = int(x)
			if ix == 0:
				x = x+a[basepos+1+i:basepos+2+i]
				#if ir != 0:
				lji.append(int(0))
				lyu.append(x)
				#lyu[len(lyu)-1] = x
			else:
				ir,iy = getshang(ix,ib)
				#print("x=",x,",res=",res,",i=",str(i),",ir="+str(ir)+",iy=",str(iy))
				res[basepos+i] = str(ir)
				x = str(iy)+a[basepos+1+i:basepos+2+i]
				#if ir != 0:
				lji.append(int(ir*ib))
				lyu.append(x)
		shang = int(''.join(res))
		yu = int(iy)
	print("result:",shang,yu,lji,lyu)
	print("%21s"%shang)
	print(' '*10+'-'*11)
	print("%10s)%10s"%(b,a))
	
	identlen = 21-lena+lenb
	inZero = False
	for i in range(len(lji)): 
		#print("identlen=",identlen)
		if lji[i] != 0:
			print("%s"%(str(lji[i]).rjust(identlen)))
			print("%s"%(''.join(['-']*len(str(lji[i]))).rjust(identlen)))
			#identlen+=lenb-len(str(lyu[i]))
		if int(lyu[i]) != 0:
			if i == len(lji) -1:  #For the last one , we do not need to shift to right .
				print("%s"%(str(lyu[i]).rjust(identlen)))
			else:
				print("%s"%(str(lyu[i]).rjust(identlen+1)))
		identlen+=1
def printout(a,f,b):
	if f == 1:
		printoutadd(a,b)
	elif f == 2:
		printoutsub(a,b)
	elif f == 3:
		printoutmul(a,b)
	elif f == 4:
		printoutdiv(a,b)
if __name__ == '__main__':
	cmd = input("Input:")
	op = cmd.find("+")
	opf = 0
	opr = -1
	
	if op > 0:
		opf = 1
		opr = op
	else:
		op = cmd.find("-")
		if op > 0:
			opf = 2
			opr = op
		else:
			op = cmd.find("x")
			if op > 0:
				opf = 3
				opr = op
			else:
				op = cmd.find("/")
				if op > 0:
					opf = 4
					opr = op
	errinput = False
	if op > 0:
		try:
			opa = cmd[0:opr]
			opb = cmd[opr+1:]
			oopa = int(opa)
			oopb = int(opb)
		except Exception:
			errinput = True
	if opf == 0 or errinput:
		print("You input a wrong expression!")
		exit(1)
	printout(opa,opf,opb)
