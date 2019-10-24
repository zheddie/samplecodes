#!/usr/bin/python
#%%
# Copyright 2015 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

# Create the model
from tensorflow.examples.tutorials.mnist import input_data
import tensorflow as tf
import numpy as np
from PIL import Image
import matplotlib.pyplot as plt

np.set_printoptions(threshold=np.inf,linewidth=200,precision=2)
#np.set_printoptions(threshold=np.inf,linewidth=200)
mnist = input_data.read_data_sets("MNIST_data/", one_hot=True)
sess = tf.InteractiveSession()
#print mnist.test.labels
in_units = 784
h1_units = 300
W1 = tf.Variable(tf.truncated_normal([in_units, h1_units], stddev=0.1))
b1 = tf.Variable(tf.zeros([h1_units]))
W2 = tf.Variable(tf.zeros([h1_units, 10]))
b2 = tf.Variable(tf.zeros([10]))

x = tf.placeholder(tf.float32, [None, in_units])
keep_prob = tf.placeholder(tf.float32)

hidden1 = tf.nn.relu(tf.matmul(x, W1) + b1)
hidden1_drop = tf.nn.dropout(hidden1, keep_prob)
y = tf.nn.softmax(tf.matmul(hidden1_drop, W2) + b2)

# Define loss and optimizer
y_ = tf.placeholder(tf.float32, [None, 10])

saver = tf.train.Saver()
saver.restore(sess, "model/MLP.ckpt")
x_out = sess.run(x,feed_dict={x:mnist.test.images})

y_out = sess.run(y_,feed_dict={y_:mnist.test.labels})
np_xout = np.array(x_out)
np_yout = np.array(y_out)

for i in range(10):
	np_xout_pic = np.reshape(np_xout[i,],(28,28))
	#%matplotlib inline
	#imgplot = plt.imshow(np_xout_pic)
	#imgplot.set_cmap('winter')
	#plt.show(imgplot)
	#full = np.full((28,28),255,dtype=np.int32)

	np_xout_l = np_xout_pic*255
	np_xout_l_int = np_xout_l.astype(np.int32)
	#np_xout_l.dtype = np.int32
	#np.cast(np_xout_l,np.int32)
	im  = Image.fromarray(np_xout_l_int)
	
	im.save("mnist.test."+str(i)+".png")
	for j in range(28):
            for k in range(28):
                if(np_xout_l_int[j,k] > 0.0):
                    print "{:.2f}".format(np_xout_l_int[j,k]),
                else:
                    print '     ',
            print ''
	#print np_xout_pic
	print np_yout[i,]
	print "=================================="
#print np_xout[1:2,]
#for i in range(x)

# Test trained model
correct_prediction = tf.equal(tf.argmax(y, 1), tf.argmax(y_, 1))
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
#print(accuracy.eval({x: mnist.test.images, y_: mnist.test.labels, keep_prob: 1.0}))