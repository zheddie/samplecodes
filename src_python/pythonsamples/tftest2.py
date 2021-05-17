import tensorflow as tf
a = tf.constant([2,4])
b = tf.multiply(a,a)
c = tf.Variable(tf.zeros([10]))
with tf.Session() as ss:
	ss.run(tf.global_variables_initializer())
	print ss.run(b)
	print ss.run(c)
