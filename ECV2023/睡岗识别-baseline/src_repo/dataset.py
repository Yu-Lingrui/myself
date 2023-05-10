import torch
from torch.utils.data import Dataset
import random
import numpy as np
import cv2
import os
import read

def resize_image(srcimg, image_size, keep_ratio=False):
    top, left, newh, neww = 0, 0, image_size, image_size
    if keep_ratio and srcimg.shape[0] != srcimg.shape[1]:
        hw_scale = srcimg.shape[0] / srcimg.shape[1]
        if hw_scale > 1:
            newh, neww = image_size, int(image_size / hw_scale)
            img = cv2.resize(srcimg, (neww, newh), interpolation=cv2.INTER_AREA)
            left = int((image_size - neww) * 0.5)
            img = cv2.copyMakeBorder(img, 0, 0, left, image_size - neww - left, cv2.BORDER_CONSTANT, value=0)  # add border
        else:
            newh, neww = int(image_size * hw_scale), image_size
            img = cv2.resize(srcimg, (neww, newh), interpolation=cv2.INTER_AREA)
            top = int((image_size - newh) * 0.5)
            img = cv2.copyMakeBorder(img, top, image_size - newh - top, 0, 0, cv2.BORDER_CONSTANT, value=0)
    else:
        img = cv2.resize(srcimg, (image_size, image_size), interpolation=cv2.INTER_AREA)
    return img, newh, neww, top, left

class Data_Name_Color():
    def __init__(self, labels_txt):
        self.class_names = list(map(lambda x: x.strip(), open(labels_txt, 'r').readlines()))
        self.cats_to_ids = dict(map(reversed, enumerate(self.class_names)))
        self.ids_to_cats = dict(enumerate(self.class_names))
        self.num_classes = len(self.class_names)
        self.colors = [[random.randint(0, 255) for _ in range(3)] for _ in range(len(self.class_names))]

def build_ev_dataset(args):
    dataroot  = '/home/data'
    all_lablist = []
    for name in os.listdir(dataroot):
        if name.isdigit():
            imgroot = os.path.join(dataroot, name)
            xmllist = list(filter(lambda x: x.endswith('.json'), os.listdir(imgroot)))
            xmllist = list(map(lambda x: os.path.join(imgroot, x), xmllist))
            all_lablist.extend(list(filter(lambda x: os.path.exists(os.path.splitext(x)[0]+'.jpg'), xmllist)))

    class_names = ["stand", "sit", "crouch", "prostrate_sleep", "sit_sleep", "lie_sleep"]   ###可以手动输入设置，也可以程序自动获取

    if os.path.exists(args.labels_txt):
        os.remove(args.labels_txt)
    wtxt = open(args.labels_txt, 'w')
    for name in class_names:
        wtxt.write(name + '\n')
    wtxt.close()

    train_dataset = EV_JSONDataset(all_lablist, args.labels_txt, image_size=args.imgsize, input_norm=args.input_norm, keep_ratio=args.keep_ratio, augment=args.augment)
    return train_dataset


class EV_JSONDataset(Dataset):
    def __init__(self, xml_list, labels_txt, image_size=416, input_norm=False, keep_ratio=False, augment=False, keep_difficult=False):
        self.image_size = image_size  ###square
        self.xml_list = xml_list
        self.nSamples = len(self.xml_list)
        self.keep_ratio = keep_ratio
        self.augment = augment
        self.input_norm = input_norm
        self.class_names = list(map(lambda x: x.strip(), open(labels_txt, 'r').readlines()))
        self.num_classes = len(self.class_names)
        self.keep_difficult = keep_difficult
    def __len__(self):
        return self.nSamples

    def __getitem__(self, index):
        jsonpath = self.xml_list[index]
        imgpath = os.path.splitext(jsonpath)[0]+'.jpg'
        srcimg = cv2.imread(imgpath)
        img, newh, neww, padh, padw = resize_image(srcimg, self.image_size, keep_ratio=self.keep_ratio)
        ratioh, ratiow = newh / (srcimg.shape[0]), neww / (srcimg.shape[1])
        dh, dw = 1. / (img.shape[0]), 1. / (img.shape[1])
        
        labels = np.empty((0,5), dtype=np.float32)
        f = open(jsonpath, 'rb')
        infos = read.load(f)
        for info in infos['annotations']:
            box_name = info['category_name']
            cat = float(self.class_names.index(box_name))
            xmin, ymin, width, height = info['bbox']
            xmax = xmin + width
            ymax = ymin + height
            
            xmin = xmin * ratiow + padw
            ymin = ymin * ratioh + padh
            xmax = xmax * ratiow + padw
            ymax = ymax * ratioh + padh
            
            obj_arr = np.array(
                [[cat, (xmin + xmax) * 0.5 * dw, (ymin + ymax) * 0.5 * dh, (xmax - xmin) * dw, (ymax - ymin) * dh]],
                dtype=np.float32)
            labels = np.append(labels, obj_arr, axis=0)

        # labels = labels.reshape(-1, 5)
        nL = labels.shape[0]  # number of labels
        if self.augment and nL > 0:
            src_labels = labels.copy()
            p = random.random()
            if 0.25 < p <= 0.5:  ### rotate 90
                img = cv2.transpose(img)
                img = cv2.flip(img, 1)
                labels[:, 1] = 1 - src_labels[:, 2]
                labels[:, 2] = src_labels[:, 1]
                labels[:, 3] = src_labels[:, 4]
                labels[:, 4] = src_labels[:, 3]
            elif p <= 0.25:  ### rotate 270
                img = cv2.transpose(img)
                img = cv2.flip(img, 0)
                labels[:, 1] = src_labels[:, 2]
                labels[:, 2] = 1 - src_labels[:, 1]
                labels[:, 3] = src_labels[:, 4]
                labels[:, 4] = src_labels[:, 3]

            if 0.25 < p <= 0.5:   # random left-right flip
                img = np.fliplr(img)
                if nL:
                    labels[:, 1] = 1 - labels[:, 1]
            elif p<=0.25:    # random up-down flip
                img = np.flipud(img)
                if nL:
                    labels[:, 2] = 1 - labels[:, 2]
        labels_out = torch.zeros((nL, 6))
        if nL:
            labels_out[:, 1:] = torch.from_numpy(labels)
        img = np.ascontiguousarray(img, dtype=np.float32)
        if self.input_norm:
            img /= 255.0
        return torch.from_numpy(img), labels_out

    def collate_fn(self, batch):
        img, label = list(zip(*batch))  # transposed
        for i, l in enumerate(label):
            l[:, 0] = i  # add target image index for build_targets()
        return torch.stack(img, 0), torch.cat(label, 0)
