import numpy as np
import csv
import random
from sklearn.naive_bayes import GaussianNB
from sklearn import metrics

def read_csv():
    dataSet = list(csv.reader(open("dataset_discrete_numeric.csv")))
    return dataSet[1:]

def toTarget(dataSet):
    newSet = []
    targetSet = []
    for row in dataSet:
        nowrow = []
        for i in range(len(row) - 1):
            nowrow.append(int(row[i]))
        newSet.append(nowrow)
        targetSet.append(int(row[-1]))
    return np.array(newSet), np.array(targetSet)

def randomSpilt(dataSet, ratio, rest = False):
    random.shuffle(dataSet)
    thres = int(len(dataSet) * ratio)
    newSet = dataSet[:thres]
    restSet = dataSet[thres:]
    if(rest):
        return newSet, restSet
    else:
        return newSet

def main():
    ds = read_csv()
    trainSet, testSet = randomSpilt(ds, 0.7, True)
    trainData, trainTarget = toTarget(trainSet)
    testData, testTarget = toTarget(testSet)

    clf = GaussianNB()
    clf = clf.fit(trainData, trainTarget)
    
    y_pred=clf.predict(testData)

    total = testData.shape[0]
    correct = (testTarget == y_pred).sum()
    print("total： %d correct : %d" % (total, correct))
    print("acc： %f" % (correct / total))

    # accuracy = metrics.accuracy_score(testTarget, y_pred)
    # print(accuracy)

if __name__ == "__main__":
    main()