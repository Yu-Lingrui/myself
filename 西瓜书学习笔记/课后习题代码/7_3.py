import pandas as pd
import numpy as np
import pprint

dis = ['色泽', '根蒂', '敲声', '纹理', '脐部', '触感']
con = ['密度', '含糖率']
lab = ['好瓜']


def naive_bayes_clssifier(df):
    # 获取标签的属性值
    label =list(set(df[lab[0]]))
    disvalue = [list(set(df[d])) for d in dis]

    # 计算正负样本数
    ta, tb = 0, 0
    for i in range(len(df[lab[0]])):
        if df[lab[0]][i] == label[0]:
            ta += 1
        else:
            tb += 1
    a = [ta, tb]

    dictss = {}

    # 计算正负样本的概率
    dict = {}
    for i, la in enumerate(label):
        dict[la] = (a[i] + 1) / (len(df[lab[0]])+len(label))
    dictss[lab[0]] = dict

    # 计算离散的拉普拉斯修正概率
    for i, dvs in enumerate(disvalue):
        dicts = {}
        for dv in dvs:
            dict = {}
            for j, la in enumerate(label):
                b = 0
                for k in range(len(df[dis[i]])):
                    if df[dis[i]][k] ==  dv and df[lab[0]][k] == la:
                        b += 1
                dict[la] = (b+1) / (a[j]+len(dvs))
            dicts[dv] = dict
        dictss[dis[i]] = dicts

    # 计算连续值均值方差
    for i, co in enumerate(con):
        dicts = {}
        for j, la in enumerate(label):
            dict = {}
            c = []
            for k in range(len(df[co])):
                if df[lab[0]][k] == la:
                    c.append(df[co][k])
            dict['均值'] = np.mean(c)
            dict['方差'] = np.var(c)
            dicts[la] = dict
        dictss[co] = dicts
    return dictss


def predict(dfs, pdict):
    pred = []
    for df in dfs:
        dist = {}
        for la in pdict[lab[0]]:
            tmp = 1
            tmp *= pdict[lab[0]][la]
            for d in df:
                if d in dis:
                    tmp *= pdict[d][df[d]][la]
                else:
                    tmp *= np.exp(-((df[d] - pdict[d][la]['均值']) ** 2) / (2 * pdict[d][la]['方差'])) / (np.sqrt(2 * np.pi * pdict[d][la]['方差']))
            dist[la] = tmp
    pred.append(dist)
    return pred


if __name__ == '__main__':
    # 读取文件
    df = pd.read_csv('watermelon.csv')
    # 获取概率字典
    pdict = naive_bayes_clssifier(df)
    pp = pprint.PrettyPrinter(indent=2)
    pp.pprint(pdict)
    # 获得测试集概率
    test = [{'色泽': '青绿', '根蒂': '蜷缩', '敲声': '浊响', '纹理': '清晰',
             '脐部': '凹陷', '触感': '硬滑', '密度': 0.697, '含糖率': 0.460}]
    result = predict(test, pdict)[0]
    print('预测为好瓜的概率：%.4f %%' % (result['是']*100))
    print('预测为坏瓜的概率：%.4f %%' % (result['否']*100))
    # 获得测试集分类
    if  result['是'] >  result['否']:
        print('Test1 为好瓜')
    else:
        print('Test1 为坏瓜')