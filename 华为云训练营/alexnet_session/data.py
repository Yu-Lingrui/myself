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
import math
import time
from datetime import datetime
import os
from PIL import Image, ImageDraw, ImageFont
import numpy as np
from tqdm import tqdm
import matplotlib.pyplot as plt

class Data:
    def __init__(self, batch_size, num_classes, data_path, val_data):
        """
        :param batch_size: batch size
        :param num_classes: number of classes for datasets
        :param data_path: training set path
        :param val_data: validation set path
        """
        self.batch_size = batch_size
        self.num_classes = num_classes
        self.data_path = data_path
        self.labels_name = []
        self.val_data = val_data
        self.image_names = os.listdir(self.data_path)
        for name in tqdm(self.image_names):
            class_name = name.split('.')[0].split('_')[-1]
            self.labels_name.append(class_name)
        class_set = set(self.labels_name)
        self.labels_dict = {}
        for v, k in enumerate(class_set):
            self.labels_dict[k] = v
        print("Data Loading finished!")
        print("Label dict: ", self.labels_dict)
        self.labels = [self.labels_dict.get(k) for k in self.labels_name]
        self.inverse_dict = dict((val, key) for key, val in self.labels_dict.items())

    # convert label to one hot form
    def getOneHotLabel(self, labels, depth):
        label = np.zeros([len(labels), depth])
        for i in range(len(labels)):
            label[i][labels[i]] = 1
        return label


    def get_batch(self, count):
        start = count * self.batch_size
        end = (count + 1) * self.batch_size
        start_pos = max(0, start)
        end_pos = min(end, len(self.labels))
        images_name_batch = self.image_names[start_pos: end_pos]
        images = []
        for images_name in images_name_batch:
            image_path = os.path.join(self.data_path, images_name)
            image = Image.open(image_path)
            image = np.array(image) / 255.0
            images.append(image)
        labels = self.labels[start_pos: end_pos]
        datas = np.array(images)
        labels = np.array(labels)
        labels = self.getOneHotLabel(labels, self.num_classes)
        return datas, labels

    def get_batch_num(self):
        return len(self.labels) // self.batch_size

    def get_batch_size(self):
        return self.batch_size

    def get_val_data(self):
        val_names = os.listdir(self.val_data)
        val_images = []
        val_labels = []
        for name in val_names:
            image_path = os.path.join(self.val_data, name)
            image = Image.open(image_path)
            image = np.array(image) / 255.0
            val_images.append(image)
            class_name_val = name.split('.')[0].split('_')[-1]
            val_labels.append(class_name_val)
        val_images = np.array(val_images)
        val_labels = [self.labels_dict.get(k) for k in val_labels]
        val_labels = np.array(val_labels)
        val_labels = self.getOneHotLabel(val_labels, self.num_classes)
        return val_images, val_labels

    def get_label_dict(self):
        return self.labels_dict

