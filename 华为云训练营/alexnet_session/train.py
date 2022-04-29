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
import os
import numpy as np
import shutil
import datetime

import tensorflow as tf
from data import Data
from model import alexnet
from cfg import make_config

flags = tf.flags
FLAGS = flags.FLAGS

## Required parameters
flags.DEFINE_string("result", "result", "The result directory where the model checkpoints will be written.")
flags.DEFINE_string("dataset", "dataset", "dataset path")
flags.DEFINE_string("obs_dir", "obs://zhonglin-public/log", "obs result path, not need on gpu and apulis platform")

## Other parametersresult
flags.DEFINE_integer("num_classes", 5, "number of classes for flowers datasets")
flags.DEFINE_float("learning_rate", 1e-3, "The initial learning rate for Adam.")
flags.DEFINE_integer("batch_size", 32, "batch size for one NPU")
flags.DEFINE_integer("train_step", 150, "total epochs for training")
flags.DEFINE_integer("save_step", 5, "epochs for saving checkpoint")
flags.DEFINE_integer("decay_step", 500, "update the learning_rate value every decay_steps times")
flags.DEFINE_float("decay_rate", 0.9, "momentum used in optimizer")
flags.DEFINE_string("resume_path", None, "checkpoint path")
flags.DEFINE_string("chip", "npu", "Run on which chip, (npu or gpu or cpu)")
flags.DEFINE_string("platform", "apulis", "Run on apulis/modelarts platform. Modelarts Platform has some extra data copy operations")

## The following params only useful on NPU chip mode
flags.DEFINE_boolean("npu_dump_data", False, "dump data for precision or not")
flags.DEFINE_boolean("npu_dump_graph", False, "dump graph or not")
flags.DEFINE_boolean("npu_profiling", False, "profiling for performance or not")
flags.DEFINE_boolean("npu_auto_tune", False, "auto tune or not. And you must set tune_bank_path param.")


def main(_):
    tf.logging.set_verbosity(tf.logging.INFO)
    tf.logging.info("**********")
    print("===>>>dataset:{}".format(FLAGS.dataset))
    print("===>>>result:{}".format(FLAGS.result))
    print("===>>>train_step:{}".format(FLAGS.train_step))

    ## print all parameters
    for attr, flag_obj in sorted(FLAGS.__flags.items()):
        print("{} = {}".format(attr.lower(), flag_obj.value))

    ##============Obtain Data================
    data = Data(batch_size=FLAGS.batch_size, num_classes=FLAGS.num_classes,
                data_path=os.path.join(FLAGS.dataset, "train"),
                val_data=os.path.join(FLAGS.dataset, "val"))
    tf.logging.info("Label dict = %s", data.labels_dict)

    x = tf.placeholder(dtype=tf.float32, shape=[None, 224, 224, 3])
    y = tf.placeholder(dtype=tf.int32, shape=[None, FLAGS.num_classes])

    # construction model
    pred = alexnet(x, FLAGS.chip.lower(), class_num=FLAGS.num_classes)

    # define loss function and optimizer
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=pred, labels=y))
    global_step = tf.Variable(0, trainable=False)
    learning_rate = tf.train.exponential_decay(FLAGS.learning_rate, global_step, FLAGS.decay_step, FLAGS.decay_rate, staircase=True)
    optimizer = tf.train.AdamOptimizer(learning_rate).minimize(cost,global_step=global_step)

    # definition accuracy
    prediction_correction = tf.equal(tf.cast(tf.argmax(pred, 1), dtype=tf.int32),
                                     tf.cast(tf.argmax(y, 1), dtype=tf.int32), name='prediction')
    accuracy = tf.reduce_mean(tf.cast(prediction_correction, dtype=tf.float32), name='accuracy')

    tf.summary.scalar('loss', cost)
    tf.summary.scalar('accuracy', accuracy)
    summary_op = tf.summary.merge_all()

    config = make_config(FLAGS)

    # start training
    with tf.Session(config=config) as sess:
        init_op = tf.group(tf.local_variables_initializer(), tf.global_variables_initializer())
        sess.run(init_op)

        ## Save graph file
        # tf.io.write_graph(sess.graph_def, './', 'graph.pbtxt')
        # tf.logging.info("Write graph pbtxt file.")

        train_writer = tf.summary.FileWriter(logdir=os.path.join(FLAGS.result, "train"), graph=sess.graph)
        test_writer = tf.summary.FileWriter(logdir=os.path.join(FLAGS.result, "test"), graph=sess.graph)

        # saver is used to save the model
        saver = tf.train.Saver(tf.global_variables(), max_to_keep=5)
        if FLAGS.resume_path is not None:
            tf.logging.info('Loading checkpoint from {}...'.format(FLAGS.resume_path))
            model_file=tf.train.latest_checkpoint(FLAGS.resume_path)
            saver.restore(sess, model_file)
            
        max_acc = 0
        for step in range(FLAGS.train_step):
            batch_num = data.get_batch_num()
            tf.logging.info(" step = %d, batch_num=%d", step, batch_num)
            train_accuracy_list = []
            train_loss_list = []
            for batch_count in range(batch_num):
                start = datetime.datetime.now()
                train_images, train_labels = data.get_batch(batch_count)
                end = datetime.datetime.now()
                data_deltatime = (end - start).total_seconds() * 1000

                ## If we want to compare the result bettwen npu and gpu, make sure the input is the same as on different plateform
                # train_images = 0.5 * np.ones((32, 224, 224, 3), dtype=np.float32)
                # train_labels = np.ones((32, 5), dtype=np.float32)
                
                start = datetime.datetime.now()
                feed_dict = {x: train_images, y: train_labels}
                _, train_loss, train_acc, summary = sess.run([optimizer, cost, accuracy, summary_op], feed_dict=feed_dict)
                end = datetime.datetime.now()
                train_deltatime = (end - start).total_seconds() * 1000
                if batch_count % 30 == 0:
                    tf.logging.info("Time Used===>>>[FP+BP]:{:.3f} (ms), [Get Data]:{:.3f} (ms)\n".format(train_deltatime, data_deltatime))
                train_loss_list.append(train_loss)
                train_accuracy_list.append(train_acc)
            
            train_writer.add_summary(summary, step)
            tf.logging.info("train_acc = %s", np.mean(train_accuracy_list))
            tf.logging.info("train_loss = %s", np.mean(train_loss_list))
            if (step + 1) % FLAGS.save_step == 0:
                test_accuracy_list = []
                test_loss_list = []
                val_images, val_labels = data.get_val_data()
                val_feed = {x: val_images, y: val_labels}

                prediction_correction = tf.equal(tf.cast(tf.argmax(pred, 1), dtype=tf.int32),
                                                 tf.cast(tf.argmax(y, 1), dtype=tf.int32), name='prediction')
                accuracy = tf.reduce_mean(tf.cast(prediction_correction, dtype=tf.float32), name='accuracy')
                test_loss, test_acc, summary = sess.run([cost, accuracy, summary_op], feed_dict=val_feed)
                test_accuracy_list.append(test_acc)
                test_loss_list.append(test_loss)
                test_writer.add_summary(summary, step)
                tf.logging.info("test_acc = %s", test_accuracy_list)
                tf.logging.info("test_loss = %s", test_loss_list)

                # save model
                if test_acc > max_acc:
                    saver.save(sess=sess, save_path=os.path.join(FLAGS.result, "model"))
                    test_writer.add_summary(summary=summary, global_step=step)
                    max_acc = test_acc

        train_writer.close()
        test_writer.close()

    if FLAGS.platform.lower() == 'modelarts':
        from help_modelarts import modelarts_result2obs
        modelarts_result2obs(FLAGS)

if __name__ == "__main__":
    flags.mark_flag_as_required("dataset")
    flags.mark_flag_as_required("result")
    flags.mark_flag_as_required("obs_dir")
    tf.app.run()