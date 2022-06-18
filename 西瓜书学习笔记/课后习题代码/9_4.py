import math
import pandas as pd
import random
import matplotlib.pyplot as plt


# 计算欧式距离
def getEuclidean(point1, point2):
    dimension = len(point1)
    dist = 0.0
    for i in range(dimension):
        dist += (point1[i] - point2[i]) ** 2
    return math.sqrt(dist)


# K均值聚类算法
def k_means(dataset, k):
    # 输入：数据、k
    # 输出：输出向量，聚类标签，类别

    # 初始化簇心向量
    index = random.sample(list(range(len(dataset))), k)
    vectors, vec = [], []
    for i in index:
        vectors.append(dataset[i])
        vec.append(dataset[i])

    # 初始化标签
    labels = []
    for i in range(len(dataset)):
        labels.append(-1)
    # 根据迭代次数重复k-means聚类过程
    for iter in range(2):
        # 初始化簇
        C = []
        for i in range(k):
            C.append([])
        for labelIndex, item in enumerate(dataset):
            classIndex = -1
            minDist = 1e6
            for i, point in enumerate(vectors):
                dist = getEuclidean(item, point)
                if (dist < minDist):
                    classIndex = i
                    minDist = dist
            C[classIndex].append(item)
            labels[labelIndex] = classIndex
        for i, cluster in enumerate(C):
            clusterHeart = []
            dimension = len(dataset[0])
            for j in range(dimension):
                clusterHeart.append(0)
            for item in cluster:
                for j, coordinate in enumerate(item):
                    clusterHeart[j] += coordinate / len(cluster)
            vectors[i] = clusterHeart
    return vec, labels

# 可视化
def show(dataset, vec, label):
    k = len(set(label))

    # 提取点
    point = []
    for i in range(k):
        point.append([[],[]])
    for i in range(len(dataset)):
        point[label[i]][0].append(dataset[i][0])
        point[label[i]][1].append(dataset[i][1])

    # 提取初始向量
    vv = [[],[]]
    for i in range(len(vec)):
        vv[0].append(vec[i][0])
        vv[1].append(vec[i][1])

    # 初始向量可视化
    plt.scatter(vv[0], vv[1],  label='Initial Vector', marker='*', s=200)

    # 分类结果可视化
    for i, p in enumerate(point):
        plt.scatter(p[0], p[1], label='class'+str(i+1))
    plt.title('k='+str(k))
    plt.legend()
    plt.show()

if __name__ == '__main__':
    df = pd.read_csv('watermelon4.csv')
    data = df[['density', 'sugercontent']].values

    # 探究不同的k值对实验的影响
    ks = [2, 4, 6]
    for k in ks:
        vec, label = k_means(data, k)
        show(data, vec, label)

    # 探究不同的初始中心点对实验的影响
    k = 4
    for i in range(3):
        vec, label = k_means(data, k)
        print(vec)
        show(data, vec, label)


