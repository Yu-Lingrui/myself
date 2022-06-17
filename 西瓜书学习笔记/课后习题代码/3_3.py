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

"""# 定义 sigmoid 函数"""

def sigmoid(Z):
    s = 1 / (1 + np.exp(-Z))
    return s

"""# 定义对率回归函数"""

def logit_regression(x, y, params, lr):
    # forward
    m = x.shape[0]
    w, b = params['w'], params['b']

    pred = sigmoid(np.dot(x, w) + b)
    cost = - 1/m * np.sum(y*np.log(pred) + (1 - y)*np.log(1 - pred))

    # backward
    ## 求导
    dw = 1/m * np.dot(x.T, (pred - y))
    db = 1/m * np.sum(pred - y)

    ## 回传
    w = w - lr * dw
    b = b - lr * db

    params['w'], params['b'] = w, b
    return cost, params

"""# 定义训练函数"""

def train(x, y, params):
    costs = []
    for i in range(EPOCH):
        cost, params = logit_regression(x, y, params, LR)
        costs.append(cost)
    return costs, params

"""# 主函数"""

# 训练前各参数
params = init_params(x)
print('the weight before training is %.4f' % params['w'][0], ' %.4f' % params['w'][1])
print('the bias before training is %.4f' % params['b'])

# 训练前分界线
plt.figure()
plt.scatter(wm['density'][:8], wm['sugar'][:8], color='red', label='positive')
plt.scatter(wm['density'][8:], wm['sugar'][8:], color='blue', label='negative')
plt.legend()
x0 = np.linspace(0, 1, 100)
# z = w0*x0 + w1*x1 + b
x1 = (- x0 * params['w'][0] - params['b']) / params['w'][1]
plt.plot(x0, x1)
plt.title('before train')
plt.show()

# 训练后各参数
costs, params=train(x, y, params)
plt.figure()
plt.plot(costs)
plt.title('loss')
plt.show()
print('the weight after training is %.4f' % params['w'][0], ' %.4f' % params['w'][1])
print('the bias after training is %.4f' % params['b'])

# 训练后分界线
plt.figure()
plt.scatter(wm['density'][:8], wm['sugar'][:8], color='red', label='positive')
plt.scatter(wm['density'][8:], wm['sugar'][8:], color='blue', label='negative')
plt.legend()
x0 = np.linspace(0, 1, 100)
# z = w0*x0 + w1*x1 + b
x1 = (- x0 * params['w'][0] - params['b']) / params['w'][1]
plt.plot(x0, x1)
plt.title('after train')
plt.show()