import tensorflow as tf
import tf2onnx

with tf.Session() as sess:
    new_saver = tf.train.import_meta_graph('model/Vnet3dModule.pd.meta')
    new_saver.restore(sess, tf.train.latest_checkpoint('model/'))
    onnx_graph = tf2onnx.tfonnx.process_tf_graph(sess.graph, output_names=["output:0"])
    model_proto = onnx_graph.make_model("Vnet3")
    with open("./vnet3d.onnx", "wb") as f:
        f.write(model_proto.SerializeToString())
