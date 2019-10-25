import tensorflow as tf
#from tensorflow.python import pywrap_tensorflow
#import os
#import tf2onnx

#reader = tf.train.NewCheckpointReader('model/Vnet3dModule.pd')
#help(reader)
#var_to_shape_map=reader.get_variable_to_shape_map()
#for key in var_to_shape_map:
#    print('tensor_name: ',key)


tf.reset_default_graph()
with tf.Session() as sess:
    saver = tf.train.import_meta_graph('model/Vnet3dModule.pd.meta')
    graph_def = tf.get_default_graph().as_graph_def()
    node_list=[n.name for n in graph_def.node]
print(node_list);
