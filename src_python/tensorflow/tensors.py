import tensorflow as tf 
sess = tf.Session()
x = tf.constant([[2,5,3,-5],[0,3,-2,5],[4,3,5,3],[6,1,4,0]])
floatx = tf.constant([[2.,5.,3.,-5.],[0.,3.,-2.,5.],[4.,3.,5.,3.],[6.,1.,4.,0.]])
floatz = tf.constant([[1.,-1.],[-1.,1.]])
with sess.as_default():
	rf = tf.matrix_inverse(floatx).eval()
	print ("rf=",rf)
	mx = tf.matmul(floatx,rf).eval()
	#mmx = sess.run(mx)
	print ("mx=",mx)
	det = tf.matrix_determinant(floatz).eval()
	print ("det=",det)
