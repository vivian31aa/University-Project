import random
from random import seed
import numpy as np
import matplotlib.pyplot as plt
import copy
import csv
    

def getLoss(predict, target):
    return np.mean(np.absolute(predict-target))


def Sigmoid(x):
    return 1.0/(1.0 + np.exp(-(np.clip(x, -100, 100))))
 
                
def Sigmoid_Deriv(x):   #input Sigmoid and output its differential
    return x * (1.0 - x)

def getAcc(predict, target):
    correct = 0
    for i in range(len(target)):
        if(predict[0][i] <= 0.5):
            a = 0
        else:
            a = 1
        if (a == target[i]):
            correct += 1
    return correct / len(target)

def showPlot(label, datax, datay):
    plt.suptitle(label)

    for i in range(len(datay)):

        if datay[i] <= 0.5:
            plt.plot(datax[0][i] , datax[1][i] , 'bo')
        else:
            plt.plot(datax[0][i] , datax[1][i] , 'ro')

    plt.show()

def ForwardPropagation(w,b, data):
    a = []
    for i in range(3):
        if(i==0):
            f = data
        else:
            f = a[i-1]
        a.append(Sigmoid(w[i].dot(f) + b[i]))
    return a


def Propagation(ws, bs, data, target, alpha):
    a = ForwardPropagation(ws, bs, data)

    proArr = []
    proArr.append(data.T)
    for i in range(len(a) - 1):
        proArr.append(a[i].T)

    sigD = Sigmoid_Deriv(np.array(a))
    
    predictL = []
    predictL = (a[-1] - target)[0]

    rev = []
    rev.append(np.array(predictL) * sigD[-1])
    rev.append(ws[2].T.dot(rev[-1]) * sigD[-2])
    rev.append(ws[1].T.dot(rev[-1]) * sigD[-3])
    rev.reverse()
    
    temp1 = np.ones( (len(target),1) )
    for i in range(3):
        ws[i] -= alpha * rev[i].dot(proArr[i])
        bs[i] -= alpha * np.sum(rev[i].dot(temp1))

    

def main():
    seed(123123)
    hiddenN = 50
    alpha = 0.13


    data = np.genfromtxt('data.txt', delimiter = ',')
    data = data.T
    target = copy.deepcopy(data[2])

    data = np.delete(data, 2, 0)


    ws = [ np.random.randn(hiddenN,2), np.random.randn(hiddenN, hiddenN), np.random.randn(1, hiddenN)]

    bs = [0,0,0]

    allAcc = []
    epochNum = 100000

    for i in range(epochNum):
        Propagation( ws, bs , data, target,alpha)
        if ( (i+1) % 10000 == 0 ) :
            predict = ForwardPropagation(ws, bs, data)[-1]
            loss = getLoss(predict, target)
            print('epochs ', i+1, end=' ')
            print('loss:', loss)
            allAcc.append(getAcc(predict, target))
    
    print(allAcc)




main()