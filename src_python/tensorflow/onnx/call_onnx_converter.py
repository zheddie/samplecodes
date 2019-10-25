import tensorflow as tf
import tf2onnx

with tf.Session() as sess:
    x = tf.placeholder(tf.float32, [2, 3], name="input")
    x_ = tf.add(x, x)
    _ = tf.identity(x_, name="output")
    help(tf2onnx.tfonnx.process_tf_graph)
    onnx_graph = tf2onnx.tfonnx.process_tf_graph(sess.graph, output_names=["output:0"])
    model_proto = onnx_graph.make_model("test")
    with open("./tfmodel.onnx", "wb") as f:
        f.write(model_proto.SerializeToString())
