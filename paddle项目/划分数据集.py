#划分数据集

#根据挂载的数据集制作制作标签文件，并进行划分
import random
import os
#生成train.txt和val.txt
random.seed(2020)
xml_dir  = '/home/aistudio/data/insulator/Annotations'#标签文件地址
img_dir = '/home/aistudio/data/insulator/JPEGImages'#图像文件地址
path_list = list()
for img in os.listdir(img_dir):
    img_path = os.path.join(img_dir,img)
    xml_path = os.path.join(xml_dir,img.replace('jpg', 'xml'))
    path_list.append((img_path, xml_path))
random.shuffle(path_list)
ratio = 0.9
train_f = open('/home/aistudio/data/train.txt','w') #生成训练文件
val_f = open('/home/aistudio/data/val.txt' ,'w')#生成验证文件

for i ,content in enumerate(path_list):
    img, xml = content
    text = img + ' ' + xml + '\n'
    if i < len(path_list) * ratio:
        train_f.write(text)
    else:
        val_f.write(text)
train_f.close()
val_f.close()

#生成标签文档
label = ['insulator']#设置你想检测的类别
with open('/home/aistudio/data/label_list.txt', 'w') as f:
    for text in label:
        f.write(text+'\n')
