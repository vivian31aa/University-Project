import csv
import math
import random
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
from random import sample
from math import log

weight = [0, 0, 0]
methodOp = 0

def readfile(setOp):
    if(setOp == 1):
        zeroSet = list(csv.reader(open("HW3_data/Logistic_data1-1.txt")))
        oneSet = list(csv.reader(open("HW3_data/Logistic_data1-2.txt")))
    else:
        zeroSet = list(csv.reader(open("HW3_data/Logistic_data2-1.txt")))
        oneSet = list(csv.reader(open("HW3_data/Logistic_data2-2.txt")))

    dataSet = []
    for i in range(len(zeroSet)):
        nowrow = [float(zeroSet[i][j]) for j in range(len(zeroSet[i]))]
        nowrow.append(1) #增加bias
        nowrow.append(0)
        dataSet.append(nowrow)
    for i in range(len(oneSet)):
        nowrow = [float(oneSet[i][j]) for j in range(len(oneSet[i]))]
        nowrow.append(1) #增加bias
        nowrow.append(1)
        dataSet.append(nowrow)

    return dataSet

def randomSpilt(dataSet, ratio, rest = 0):
    random.shuffle(dataSet)
    thres = int(len(dataSet) * ratio)
    newSet = dataSet[:thres]
    restSet = dataSet[thres:]
    if(rest):
        return newSet, restSet
    else:
        return newSet

def sigmoid(x):
    if x >= 600:
        return 1
    elif x <= -600:
        return 0
    return 1 / (1 + math.exp(-x))

def l2NormError(dataSet):
    error = 0.0
    for i in range(len(dataSet)):
        error +=  pow(dataSet[i][-1] - sigmoid(predict_noSig(dataSet[i])), 2) / 2
    return error

def l2Gradient(dataSet):
    newWeight = []
    for i in range(3):
        gradient = 0.0
        for j in range(len(dataSet)):
            sigPre = sigmoid(predict_noSig(dataSet[j]))
            gradient += (dataSet[j][-1] - sigPre) * sigPre * (1 - sigPre) * dataSet[j][i]
        newWeight.append(gradient)
    return newWeight

def crossEntropy(dataSet):
    entropy = 0.0
    for i in range(len(dataSet)):
        entropy -= dataSet[i][-1] * math.log(predict(dataSet[i]), math.e)

def CEGradient(dataSet):
    newWeight = []

    for i in range(3):
        gradient = 0.0
        for data in dataSet:
            gradient -= (sigmoid(predict_noSig(data)) - data[-1]) * data[i]
        gradient /= len(dataSet)
        newWeight.append(gradient)
    
    return newWeight

def predict(data):
    ans = 0
    for i in range(3):
        ans += weight[i] * data[i]
    ans = sigmoid(ans)
    return 0 if ans <= 0.5 else 1

def predict_noSig(data):
    ans = 0
    for i in range(3):
        ans += weight[i] * data[i]
    return ans

def train(dataSet, epochNum):
    # print("data = ")
    # print(dataSet)
    for epoch in range(1, epochNum):
        # print("--")
        # print(weight)
        gradient = []
        learn_rate = 0.0
        
        if methodOp == 1:
            learn_rate = 0.01
            gradient = l2Gradient(dataSet)
        else:
            learn_rate = 0.1
            gradient = CEGradient(dataSet)
        
        for i in range(3):
            weight[i] += learn_rate * gradient[i]

def getConMat(dataSet):
    conMat = [[0, 0], [0, 0]]
    for data in dataSet:
        if data[-1] == 1:
            if predict(data) == 1:
                conMat[0][0] += 1
            else:
                conMat[0][1] += 1
        else:
            if predict(data) == 1:
                conMat[1][0] += 1
            else:
                conMat[1][1] += 1
    
    return conMat

def printConMat(conMat):
    print('{:^10}'.format('[Matrix]') + '{:>10}'.format('predict1') + '   ' + '{:>10}'.format('predict0'))
    print('{:^10}'.format('actual1') + '{:10}'.format(conMat[0][0]) + '   ' + '{:10}'.format(conMat[0][1]))
    print('{:^10}'.format('actual0') + '{:10}'.format(conMat[1][0]) + '   ' + '{:10}'.format(conMat[1][1]))
    
    print("Accuracy = ", (conMat[0][0] + conMat[1][1]) / (conMat[0][0] + conMat[0][1] + conMat[1][0] + conMat[1][1]))
    print("Sensitivity = ", conMat[0][0] / (conMat[0][0] + conMat[0][1]))
    print("Recall = ", conMat[0][0] / (conMat[0][0] + conMat[1][0]))
    return

def plotInput(dataSet, setOp):
    sns.set_style("darkgrid")
    sns.set(color_codes = True)
    
    title = "data input " + str(setOp)
    myfig = sns.scatterplot(x = 'x', y = 'y', hue = 'ans', data = dataSet)
    myfig.set_title(title)
    plt.savefig('input.png')
    return

def plotOutput(dataSet, setOp):
    sns.set_style("darkgrid")
    sns.set(color_codes = True)
    
    title = "data output " + str(setOp)
    myfig = sns.scatterplot(x = 'x', y = 'y', hue = 'predict', data = dataSet)
    myfig.set_title(title)
    plt.savefig('output.png')
    return

def toPdDf(dataSet):
    dsDict = {'x' : [], 'y' : [], 'ans' : [], 'predict' :[]}
    
    dsDict['x'] = [data[0] for data in dataSet]
    dsDict['y'] = [data[1] for data in dataSet]
    dsDict['ans'] = [data[-1] for data in dataSet]
    dsDict['predict'] = [predict(data) for data in dataSet]
    
    dsDict = pd.DataFrame(dsDict)
    return dsDict

def main():
    global methodOp
    print("enter 1 for dataset 1, 2 for dataset 2")
    setOp = int(input())
    dataSet = readfile(setOp)
    print("enter method (1 for l2 norm, 2 for cross entropy):")
    methodOp = int(input())

    train(randomSpilt(dataSet, 1), 5000)
    # conMat = getConMat(dataSet)
    # printConMat(conMat)
    print(weight)
    # dsDict = toPdDf(dataSet)
    # plotInput(dsDict, setOp)
    # plotOutput(dsDict, setOp)

if __name__ == "__main__":
    main()