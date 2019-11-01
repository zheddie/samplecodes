from ptyprocess import PtyProcessUnicode
p = PtyProcessUnicode.spawn(['python'])
x = p.read(20)
print("x=",x)
p.write('6+6\n')
y = p.read(20)
print("y=",y)
