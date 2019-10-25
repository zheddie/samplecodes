import tensorflow as tf 
g = tf.Graph()
with g.as_default():
	sess = tf.Session()
	W_m = tf.Variable(tf.zeros([10,5]))
	X_v = tf.placeholder(tf.float32,[None,10])
	result = tf.matmul(X_v,W_m)
	print (g.as_graph_def())

