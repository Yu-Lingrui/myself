import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# 加载数据
def load_data(path):
    df = pd.read_csv(path)
    return df


# 计算信息熵
def get_Ent(D):
    # D为数据集，l为标签所在列的名称
    labels = [1, -1]
    Ent = 0
    if len(D) == 0:
        return 0
    for label in labels:
        p = len(D[D['好瓜'] == label]) / float(len(D))
        if p == 0:
            Ent = 0
        else:
            Ent = Ent + (p)*np.log2(p)
    return -Ent


# 计算连续信息增益
def get_continuous_Gain(D, a):
    # D为数据集，a为属性值，l为标签所在列的名称
    T =[]
    for i in range(len(D)-1):
        T.append((np.sort(D[a])[i]+np.sort(D[a])[i+1])/2)
    T = np.sort(T)
    Gains = []
    for t in T:
        Dlow = D[D[a] < t]
        Dup = D[D[a] > t]
        Gain = get_Ent(D) - (len(Dlow) / float(len(D)) * get_Ent(Dlow) + len(Dup) / float(len(D)) * get_Ent(Dup))
        Gains.append(Gain)
    return [np.max(Gains), T[np.argmax(Gains)]]


# 寻找信息增益最大的划分数据集的方式
def chooseBestSplit(D, A):
    Gains = []
    for a in A:
        Gains.append(get_continuous_Gain(D, a))
    # bestFeature: 使得到最大增益划分的属性。
    # bestThresh： 使得到最大增益划分的数值。
    bestFeature = A[(np.argmax(Gains, axis=0))[0]]
    bestThresh = Gains[(np.argmax(Gains, axis=0))[0]][1]
    return bestFeature, bestThresh


# 投票
def majorityCnt(D):
    labels = [-1, 1]
    counts = []
    for label in labels:
        count = len(D[D['好瓜'] == label])
        counts.append(count)
    return labels[np.argmax(counts)]


# 预测
def predit(df, D, bestFeature, bestThresh):
    subDL, subDR = D[D[bestFeature] < bestThresh], D[D[bestFeature] > bestThresh]
    labelL = majorityCnt(subDL)
    labelR = majorityCnt(subDR)
    pl = []
    for i in range(len(df)):
        if df[bestFeature].values[i] < bestThresh:
            pl.append(labelL)
        else:
            pl.append(labelR)
    return pl, labelL, labelR


# 计算错误率
def get_error(df, pl):
    err = 0
    for i in range(len(df)):
        if df['好瓜'].values[i] != pl[i]:
            err += 1
    return err / len(df)


# 生成决策树
def TreeGenerate(df, D):
    A = list(D.columns[:-1])
    # 得到增益最大划分的属性、阈值
    bestFeature, bestThresh = chooseBestSplit(D, A)
    predlabel, labelL, labelR = predit(df, D, bestFeature, bestThresh)
    err = get_error(df, predlabel)
    return predlabel, labelL, labelR, bestFeature, bestThresh, err


# 生成总的标签
def get_label(pls):
    label = []
    for i in range(len(pls[0])):
        tmp = 0
        for j in range(len(pls)):
            tmp += pls[j][i]
        if tmp < 0:
            label.append(-1)
        else:
            label.append(1)
    return label


# 自主采样
def boostStrap(df, m):
    a = df['密度'].values
    b = df['含糖率'].values
    l = df['好瓜'].values
    tmp = []
    for i in range(m):
        idx = int(np.floor(np.random.uniform(0, 17)))
        tmp.append([a[idx], b[idx], l[idx]])
    D = pd.DataFrame(tmp, columns=['密度', '含糖率', '好瓜'])
    return D


# Bagging
def Bagging(df, m, n):
    bestFeatures, bestThreshs, labelLs, labelRs, predlabels, errors = [], [], [], [], [], []
    for i in range(n):
        D = boostStrap(df, m)
        predlabel, labelL, labelR, bestFeature, bestThresh, err = TreeGenerate(df, D)
        if err < 0.5:
            bestFeatures.append(bestFeature)
            bestThreshs.append(bestThresh)
            labelLs.append(labelL)
            labelRs.append(labelR)
            predlabels.append(predlabel)
            errors.append(get_error(df, get_label(predlabels)))
    result = pd.DataFrame(np.array([bestFeatures, bestThreshs, labelLs, labelRs, errors]).T,
                          columns=['划分属性', '划分值', '<划分值时分类', '>划分值时分类', 'errors'])
    return result


def show(D, df):
    errors =[eval(err) for err in df['errors'].values]
    Features = df['划分属性'].values
    Threshs = [eval(t) for t in df['划分值'].values]
    plt.plot(errors)

    plt.figure()
    Rx = []
    Ry = []
    Bx = []
    By = []
    for i in range(len(D)):
        if D['好瓜'].values[i] == 1:
            Rx.append(D['密度'].values[i])
            Ry.append(D['含糖率'].values[i])
        else:
            Bx.append(D['密度'].values[i])
            By.append(D['含糖率'].values[i])
    plt.plot(Rx, Ry, 'r+')
    plt.plot(Bx, By, 'b_')

    for i in range(len(Features)):
        if Features[i] == '密度':
            plt.plot([Threshs[i], Threshs[i]], [0, 0.5])
        else:
            plt.plot([0.2, 0.8], [Threshs[i], Threshs[i]])
    plt.show()

if __name__== '__main__':
    df = load_data('watermelon3.csv')
    result = Bagging(df, 14, 10)
    print(result[['划分属性', '划分值', '<划分值时分类', '>划分值时分类']])
    show(df, result)