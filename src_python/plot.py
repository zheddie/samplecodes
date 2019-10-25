import matplotlib.pyplot as plt
import matplotlib as mpl
mpl.use("Agg")
plt.plot([1,2,3,4])
plt.ylabel('some numbers')
#plt.show()
plt.savefig("hello.png")
