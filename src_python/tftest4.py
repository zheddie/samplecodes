import tensorflow as tf
import numpy as np
data = np.random.randint(100, size=100)
print(data)

dataTF = tf.convert_to_tensor(data)
xx = tf.constant(-1,shape=[2,3])
print xx.val()

print(dataTF)
