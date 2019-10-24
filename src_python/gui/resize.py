from skimage import transform,data
import matplotlib.pyplot as plt
img = data.camera()
dst=transform.resize(img, (80, 60))
plt.figure('resize')

plt.subplot(121)
plt.title('before resize')
plt.imshow(img,plt.cm.gray)

plt.subplot(122)
plt.title('After resize')
plt.imshow(dst,plt.cm.gray)

plt.show()
