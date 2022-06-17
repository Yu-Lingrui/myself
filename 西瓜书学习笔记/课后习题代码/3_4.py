# -*- coding: utf-8 -*-


"""# 导包"""

import time
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

from sklearn.model_selection import StratifiedKFold
from sklearn.model_selection import LeaveOneOut

from google.colab import drive

"""# 定义超参数"""

N_FOLD = 10 # 折数
LR = 0.1 # 学习率
EPOCH = 10000 # 训练次数

"""# 数据预处理"""

drive.mount('/content/drive')
heart = pd.read_csv('/content/drive/MyDrive/data/UCI/heart.csv')
iris = pd.read_csv('/content/drive/MyDrive/data/UCI/iris.csv', header=None, names=['sl','sw','pl','pw','target'], nrows=100)

heart.head()

iris.head()

heart_x = heart[['age', 'sex', 'cp', 'trestbps', 'chol', 'fbs',
          'restecg', 'thalach', 'exang', 'oldpeak', 'slope',
          'ca', 'thal']].values
heart_y = heart['target'].values

iris_x = iris[['sl', 'sw', 'pl', 'pw']].values
iris_y = iris['target'].values

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

"""# 定义预测函数"""

def predict(params, x):
    m = x.shape[0]
    w, b = params['w'], params['b']
    pred = sigmoid(np.dot(x, w) + b) 
    y = []
    for i in range(m):
        # 以0.5为阈值分类
        if pred[i] > 0.5:
            y.append(1)
        else:
            y.append(0)
    return y

"""# 定义训练函数"""

def cv_train(x, y):
    preds, reals, sort_preds, sort_real, idx= [], [], [], [], []
    folds = StratifiedKFold(n_splits=N_FOLD).split(x, y)
    for fold, (trn_idx, val_idx) in enumerate(folds):
        param = init_params(x)
        for i in range(EPOCH):
            _, param = logit_regression(x[trn_idx], y[trn_idx], param, LR)
        
        preds.extend(predict(param, x[val_idx]))
        reals.extend(y[val_idx])
        idx.extend(val_idx)
    # 按照验证集索引排序，便于求平均值
    vidx = np.array(idx).argsort()
    sort_preds.append(np.array(preds)[vidx])
    sort_real.append(np.array(reals)[vidx])
    return sort_preds[0], sort_real[0]

def loo_train(x, y):
    preds, reals, sort_preds, sort_real, idx= [], [], [], [], []
    folds = LeaveOneOut().split(x, y)
    for trn_idx, val_idx in folds:
        param = init_params(x)
        for i in range(EPOCH):
            _, param = logit_regression(x[trn_idx], y[trn_idx], param, LR)
        preds.extend(predict(param, x[val_idx]))
        reals.extend(y[val_idx])
        idx.extend(val_idx)
    # 按照验证集索引排序，便于求平均值
    vidx = np.array(idx).argsort()
    sort_preds.append(np.array(preds)[vidx])
    sort_real.append(np.array(reals)[vidx])
    return sort_preds[0], sort_real[0]

"""# 计算错误率

"""

def error(pred, label):
    n = len(pred)
    t = 0
    for i in range(n):
        if pred[i] != label[i]:
            t = t + 1
    acc = t / n
    return acc

"""# 主函数"""

# heart数据集留一法预测
loo_heart_start = time.perf_counter()
loo_heart_preds, loo_heart_real = loo_train(heart_x, heart_y)
loo_heart_end = time.perf_counter()
print(loo_heart_preds)
print(loo_heart_real)

# iris数据集留一法预测
loo_iris_start = time.perf_counter()
loo_iris_preds, loo_iris_real = loo_train(iris_x, iris_y)
loo_iris_end = time.perf_counter()
print(loo_iris_preds)
print(loo_iris_real)

# heart数据集交叉验证法预测
cv_heart_start = time.perf_counter()
cv_heart_preds, cv_heart_real = cv_train(heart_x, heart_y)
cv_heart_end = time.perf_counter()
print(cv_heart_preds)
print(cv_heart_real)

# iris数据集交叉验证法预测
cv_iris_start = time.perf_counter()
cv_iris_preds, cv_iris_real = cv_train(iris_x, iris_y)
cv_iris_end = time.perf_counter()
print(cv_iris_preds)
print(cv_iris_real)

print('heart数据集留一法预测:')
print('耗时：%.4fs' % (loo_heart_end-loo_heart_start), ' | 错误率：%.4f'%(error(loo_heart_preds,loo_heart_real)))
print('iris数据集留一法预测:')
print('耗时：%.4fs' % (loo_iris_end-loo_iris_start), ' | 错误率：%.4f'%(error(loo_iris_preds, loo_iris_real)))
print('heart数据集交叉验证法预:')
print('耗时：%.4fs:' % (cv_heart_end-cv_heart_start), ' | 错误率：%.4f'%(error(cv_heart_preds, cv_heart_real)))
print('iris数据集交叉验证法预测:')
print('耗时：%.4fs:' % (cv_iris_end-cv_iris_start), ' | 错误率：%.4f'%(error(cv_iris_preds, cv_iris_real)))