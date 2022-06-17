# -*- coding: utf-8 -*-


"""# 导包"""

import numpy as np
import pandas as pd
from google.colab import drive
import matplotlib.pyplot as plt

"""# 定义超参数"""

LR = 0.1 # 学习率
EPOCH = 20000 # 训练次数

"""# 数据预处理"""

drive.mount('/content/drive')
wm = pd.read_csv('/content/drive/MyDrive/data/tmp/watermelon.csv')

wm.head()

x = wm[['density', 'sugar']].values
y = wm['label'].values

"""# 初始化参数"""

def init_params(x):
    w = np.random.randn(len(x[0]))
    b = 0
    params = {'w' : w, 'b' : b} 
    return params

"""# 定义线性判别分析"""

def LDA(x, y):
    # 0,1两类数据分开
    data1 = x[y == 1]
    data0 = x[y == 0]

    # 求得两类数据的均值向量
    mean0 = data0.mean(axis=0, keepdims=True)
    mean1 = data1.mean(axis=0, keepdims=True)
  
    # 得到两种数据的协方差矩阵
    diff1 = data1 - mean1
    diff0 = data0 - mean0
    cov1 = np.dot(diff1.T, diff1)
    cov0 = np.dot(diff0.T, diff0)
    
    # 得到类内散度矩阵
    sw = cov1 + cov0

    # 求得参数w
    swInv = np.linalg.inv(sw)
    w = np.dot(swInv, mean0.T - mean1.T)
    return w

"""# 主函数"""

w = LDA(x, y)
print('the weight is %.4f' % w[0][0], ' %.4f' % w[1][0])

# plot data points
plt.figure()
plt.scatter(wm['density'][:8], wm['sugar'][:8], color='red', label='positive')
plt.scatter(wm['density'][8:], wm['sugar'][8:], color='blue', label='negative')
plt.legend()

# plot line
x0 = np.linspace(0, 1, 100)
x1 = -w[0] * x0 / w[1]
plt.plot(x0, x1)

plt.show()