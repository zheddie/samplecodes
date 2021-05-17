from sklearn import datasets
import matplotlib.pyplot as plt
digits = datasets.load_digits()
print(digits.images.shape)
plt.imshow(digits.images[-2],cmap=plt.cm.gray_r)
plt.show()
#plt.close()
