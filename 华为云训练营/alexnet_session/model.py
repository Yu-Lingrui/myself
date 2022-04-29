# Copyright 2021 Huawei Technologies Co., Ltd
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
# coding=utf-8
import tensorflow as tf

def alexnet(input_x, chip, class_num=5, keep_prob=0.5, lrn_option=False):
    # conv1
    with tf.name_scope("conv1"):
        filter1 = tf.get_variable(name='filter1', shape=[11, 11, 3, 96],
                                  initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        conv1 = tf.nn.conv2d(input=input_x, filter=filter1, strides=[1, 4, 4, 1], padding="SAME")
        biases1 = tf.get_variable(name='biases1', shape=[96], dtype=tf.float32,initializer=tf.constant_initializer(0.0))
        layer1 = tf.nn.relu(tf.nn.bias_add(value=conv1, bias=biases1))
        print("=========conv1=======", layer1)

    # lrn
    if lrn_option:  # whether to use LRN
        layer1 = tf.nn.lrn(layer1, depth_radius=4, bias=1, alpha=0.001, beta=0.75, name='lrn1')
        print("========pool1=========", layer1)

    #pool1
    with tf.name_scope("pool1"):
        pool1 = tf.nn.max_pool(value=layer1, ksize=[1, 3, 3, 1], strides=[1, 2, 2, 1],padding='VALID', name='pool1')
        print("========lrn1=========", pool1)

    # conv2
    with tf.name_scope("conv2"):
        filter2 = tf.get_variable(name='filter2', shape=[5, 5, 96, 256], initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        conv2 = tf.nn.conv2d(input=pool1, filter=filter2, strides=[1, 1, 1, 1], padding='SAME')
        biases2 = tf.get_variable(name='biases2', shape=[256], dtype=tf.float32, initializer=tf.constant_initializer(0.0))
        layer2 = tf.nn.relu(tf.nn.bias_add(value=conv2, bias=biases2))
        print("========conv2=======",layer2)

    # lrn
    if lrn_option:
        layer2 = tf.nn.lrn(layer2, depth_radius=4, bias=1, alpha=0.001, beta=0.75, name='lrn2')
        print("========lrn2=========", layer2)

    # pool2
    with tf.name_scope("pool2"):
        pool2 = tf.nn.max_pool(value=layer2, ksize=[1, 3, 3, 1], strides=[1, 2, 2, 1], padding='VALID', name='pool1')
        print("========pool2========", pool2)

    # conv3
    with tf.name_scope("conv3"):
        filter3 = tf.get_variable(name='conv3', shape=[3, 3, 256, 384], dtype=tf.float32,initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        conv3 = tf.nn.conv2d(input=pool2, filter=filter3, strides=[1, 1, 1, 1], padding='SAME')
        biases3 = tf.get_variable(name='biases3', shape=[384], dtype=tf.float32,
                                  initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        layer3 = tf.nn.relu(tf.nn.bias_add(value=conv3, bias=biases3))
        print("========conv3========", layer3)

    # conv4
    with tf.name_scope("conv4"):
        filter4 = tf.get_variable(name='conv4', shape=[3, 3, 384, 384], dtype=tf.float32,
                                  initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        conv4 = tf.nn.conv2d(input=layer3, filter=filter4, strides=[1, 1, 1, 1], padding='SAME')
        biases4 = tf.get_variable(name='biases4', shape=[384], dtype=tf.float32,initializer=tf.constant_initializer(0.0))
        layer4 = tf.nn.relu(tf.nn.bias_add(value=conv4, bias=biases4))
        print("========conv4========", layer4)

    # conv5
    with tf.name_scope("conv5"):
        filter5 = tf.get_variable(name='conv5', shape=[3, 3, 384, 256], dtype=tf.float32,
                                  initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        conv5 = tf.nn.conv2d(input=layer4, filter=filter5, strides=[1, 1, 1, 1], padding='SAME')
        biases5 = tf.get_variable(name='biases5', shape=[256], dtype=tf.float32,
                                       initializer=tf.constant_initializer(0.0))
        layer5 = tf.nn.relu(tf.nn.bias_add(value=conv5, bias=biases5))
        print("========conv5========", layer5)

    # pool5
    with tf.name_scope("pool3"):
        layer6 = tf.nn.max_pool(value=layer5, ksize=[1, 3, 3, 1], strides=[1, 2, 2, 1], padding="VALID")
        print("========pool5========", layer6)

    # fc1
    with tf.name_scope("fc1"):
        pool_shape = layer6.get_shape().as_list()
        nodes = pool_shape[1] * pool_shape[2] * pool_shape[3]
        fc1 = tf.reshape(layer6, shape=[-1, nodes])
        fc1_weight = tf.get_variable(name='fc1_weight', shape=[nodes, 2048],
                                    initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        fc1_biases = tf.get_variable(name='fc1_biases', shape=[2048],
                                          initializer=tf.constant_initializer(0.0))
        layer6 = tf.nn.relu(tf.matmul(fc1, fc1_weight) + fc1_biases)
        if chip == 'npu':
            # modify the dropout layer
            from npu_bridge.estimator import npu_ops
            layer6 = npu_ops.dropout(layer6, keep_prob=keep_prob)
        elif chip in ['gpu', 'cpu']:
            layer6 = tf.nn.dropout(layer6, keep_prob=keep_prob)
        print("========fc1========", layer6)

    # fc2
    with tf.name_scope("fc2"):
        fc2_weight = tf.get_variable(name='fc2_weight', dtype=tf.float32, shape=[2048, 2048],
                                     initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        fc2_biases = tf.get_variable(name='fc2_biases', dtype=tf.float32, shape=[2048],
                                          initializer=tf.constant_initializer(0.0))
        layer7 = tf.nn.relu(tf.matmul(layer6, fc2_weight) + fc2_biases)
        if chip == 'npu':
            # modify the dropout layer
            from npu_bridge.estimator import npu_ops
            layer7 = npu_ops.dropout(layer7, keep_prob=keep_prob)
        elif chip in ['gpu', 'cpu']:
            layer7 = tf.nn.dropout(layer7, keep_prob=keep_prob)
        print("========fc2========", layer7)


    with tf.name_scope("output"):
        fc3_weight = tf.get_variable(name='fc3_weight', dtype=tf.float32, shape=[2048, class_num],
                                     initializer=tf.truncated_normal_initializer(mean=0, stddev=0.1))
        fc3_biases = tf.get_variable(name='fc3_biases', dtype=tf.float32, shape=[class_num],
                                     initializer=tf.constant_initializer(0.0))
        out = tf.nn.bias_add(value=tf.matmul(layer7, fc3_weight), bias=fc3_biases,
                                name='outputs')
        print("========out========", out)
    return out


