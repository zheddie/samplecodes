import tensorflow as tf
a = tf.constant([1,2,3,4,5,6])
pa = tf.Print(a,[a[0:5],a[3],a[5]],"a=")
with tf.Session() as ss:
	print ss.run(pa)
