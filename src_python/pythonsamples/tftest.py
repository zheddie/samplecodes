import tensorflow as tf
a = tf.Variable(tf.zeros([10]))
sess = tf.InteractiveSession()
tf.global_variables_initializer().run()
print sess.run(a)
