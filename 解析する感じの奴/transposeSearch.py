from keras.models import Model,Sequential
from keras.layers import Dense,Activation,Input
import numpy as np
import csv
import matplotlib.pyplot as plt
np.set_printoptions(threshold=np.inf)


def init():
    answerNum = 8
    name = "log.csv"
    data = np.loadtxt(name,delimiter=",")
    answer = np.eye(answerNum)
    return data,answer,answerNum



def main():
    network()
    
def network():
    node = 2
    teacher = np.eye(2)
    data, answer, answerNum = init()
    newdata = data[[0,2]]
    NN = Sequential()
    NN.add(Dense(49,input_dim = 48))
    NN.add(Activation("sigmoid"))
    NN.add(Dense(2))
    NN.add(Activation("softmax"))
    NN.compile(optimizer="adam", loss="categorical_crossentropy")
    NN_test = NN
    # NN.fit(newdata,np.eye(node),epochs=150)
    NN.fit(newdata,teacher,epochs=150)
    for i in range(3,len(data)-1):
    #for i in range(3, 20):
        testdata = NN.predict(data[0:i+1])
        node += 1
        if testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()] < 0.95:
            NN.pop()
            newdata = np.append(newdata, data[[testdata.shape[0]-1]].reshape(1, 48), axis=0)
            NN.add(Dense(teacher.shape[1]+1))
            NN.add(Activation("softmax"))
            NN.compile(optimizer="adam", loss="categorical_crossentropy")
            teacher = teacher.T
            array = np.zeros((1,teacher.shape[1]))
            teacher = np.r_[teacher, array]
            teacher = teacher.T
            array2 = np.zeros((1,teacher.shape[1]))
            array2[0][array2.shape[1]-1] = 1
            teacher = np.r_[teacher, array2]
        else:
            addData = np.zeros((1,testdata.shape[1]))
            addData[0][testdata[testdata.shape[0]-1].argmax()]=1
            newdata = np.append(newdata, data[[testdata.shape[0]-1]].reshape(1, 48), axis=0)
            teacher=np.r_[teacher,addData]
        history=NN.fit(newdata,teacher,epochs=int(pow(testdata.shape[1],1.5)))
        print(newdata)
        print(teacher)
    testdata = NN.predict(data)
    loss     = history.history['loss']
    nb_epoch = len(loss)
    plt.plot(range(nb_epoch), loss,     marker='.', label='loss')
    plt.legend(loc='best', fontsize=10)
    plt.grid()
    plt.xlabel('epoch')
    plt.ylabel('loss')
    plt.show()
    #print(testdata)
    ch = []
    for i in testdata:
        ch.append(np.argmax(i))
    with open('out.csv', 'w') as f:
        writer = csv.writer(f, lineterminator='\n')  # 改行コード（\n）を指定しておく
        writer.writerow(ch)     # list（1次元配列）の場合
    np.savetxt("answer.csv",newdata,delimiter=",")
    NN.summary()
        

if __name__ == '__main__':
    main()
