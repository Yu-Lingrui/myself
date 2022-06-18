import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


# 显示所有列
pd.set_option('display.max_columns', None)
# 显示所有行
pd.set_option('display.max_rows', None)
# 设置value的显示长度为100，默认为50
pd.set_option('max_colwidth', 20)


# 加载数据
def load_data(path):
    df = pd.read_csv(path)
    return df


# 计算信息熵
def get_Ent(D, w):
    pos = 0.0000000001
    neg = 0.0000000001
    for i in range(len(D)):
        if D['好瓜'].values[i] == 1:
            pos = pos + w[i]  # 标签为1的权重
        else:
            neg = neg + w[i]  # 标签为-1的权重
    P_pos = pos / (pos + neg)  # 标签为1占的比例
    P_neg = neg / (pos + neg)  # 标签为-1占的比例
    Ent = - P_pos * np.log(P_pos) - P_neg * np.log(P_neg)
    return Ent


# 计算信息增益
def get_continuous_Gain(D, a, w):
    T = []
    for i in range(len(D) - 1):
        T.append((np.sort(D[a])[i] + np.sort(D[a])[i + 1]) / 2)
    T = np.sort(T)
    Gains = []
    for t in T:
        Dlow = D[D[a] < t]
        Dup = D[D[a] > t]
        wlow, wup = [], []
        a1, a2 = 0, 0
        for j in range(len(D)):
            if D[a].values[j] < t:
                a1 += w[j]
                wlow.append(w[j])
            else:
                a2 += w[j]
                wup.append(w[j])
        Gain = get_Ent(D, w) - (a1*get_Ent(Dlow, wlow) + a2*get_Ent(Dup, wup))
        Gains.append(Gain)
    return [np.max(Gains), T[np.argmax(Gains)]]


# 寻找信息增益最大的划分数据集的方式
def chooseBestSplit(D, A, w):
    Gains = []
    for a in A:
        Gains.append(get_continuous_Gain(D, a, w))
    # bestFeature: 使得到最大增益划分的属性。
    # bestThresh： 使得到最大增益划分的数值。
    bestFeature = A[(np.argmax(Gains, axis=0))[0]]
    bestThresh = Gains[(np.argmax(Gains, axis=0))[0]][1]
    return bestFeature, bestThresh


# 投票
def majorityCnt(D, w):
    labels = [1, -1]
    counts = [0, 0]
    for i in range(len(D)):
        if D['好瓜'].values[i] == 1:
            counts[0] += w[i]
        else:
            counts[1] += w[i]
    return labels[np.argmax(counts)]


# 预测
def predit(D, bestFeature, bestThresh, w):
    subDL, subDR = D[D[bestFeature] < bestThresh], D[D[bestFeature] > bestThresh]
    labelL = majorityCnt(subDL, w)
    labelR = majorityCnt(subDR, w)
    pl = []
    for i in range(len(D)):
        if D[bestFeature].values[i] < bestThresh:
            pl.append(labelL)
        else:
            pl.append(labelR)
    return pl, labelL, labelR


# 计算错误率
def get_error(D, pl, w):
    err = 0
    for i in range(len(D)):
        if D['好瓜'].values[i] != pl[i]:
            err += w[i]
    return err


# 生成决策树
def TreeGenerate(D, w):
    A = list(D.columns[:-1])
    # 得到增益最大划分的属性、阈值
    bestFeature, bestThresh = chooseBestSplit(D, A, w)
    predlabel, labelL, labelR = predit(D, bestFeature, bestThresh, w)
    err = get_error(D, predlabel, w)
    return predlabel, err, labelL, labelR, bestFeature, bestThresh


# 生成总的标签
def get_label(pls, W):
    label = []
    for i in range(len(pls[0])):
        tmp = 0
        for j in range(len(W)):
            tmp += pls[j][i] * W[j]
        if tmp/np.sum(np.array(W)) < 0:
            label.append(-1)
        else:
            label.append(1)
    return label


# AdaBoost
def AdaBoost(df, num):
    w, W, bestFeatures, bestThreshs, labelLs, labelRs, predlabels, errors= [], [], [], [], [], [], [], []
    w.append([1 / len(df)] * len(df))
    for i in range(num):
        predlabel, err, labelL, labelR, bestFeature, bestThresh = TreeGenerate(df, w[-1])
        bestFeatures.append(bestFeature)
        bestThreshs.append(bestThresh)
        labelLs.append(labelL)
        labelRs.append(labelR)
        predlabels.append(predlabel)
        if err > 0.5:
            break
        else:
            # 计算单个树的权重
            W.append(0.5 * np.log((1 - err) / err))
            # 更新样本权重
            tmp1 = []
            for j in range(len(df)):
                tmp1.append(w[-1][j] * np.exp(-predlabel[j] * df['好瓜'].values[j] * W[-1]))
            # 样本权重归一化
            tmp2 = []
            for j in range(len(df)):
                tmp2.append(tmp1[j] / np.sum(np.array(tmp1)))
            w.append(tmp2)
            error = get_error(df, get_label(predlabels, W), [1 / len(df)] * len(df))
            errors.append(error)
            if error < 0.05:
                break
    result = pd.DataFrame(np.array([bestFeatures, bestThreshs, labelLs, labelRs, W, errors]).T,
                          columns=['划分属性', '划分值', '<划分值时分类', '>划分值时分类', '分类器权重', 'errors'])
    return result

# 画图
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
    result = AdaBoost(df, 15)
    print(result[['划分属性', '划分值', '<划分值时分类', '>划分值时分类', '分类器权重']])
    show(df, result)
