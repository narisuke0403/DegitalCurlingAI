from keras.models import Model, Sequential, model_from_json
from keras.layers import Dense, Activation, Dropout
from keras.callbacks import EarlyStopping, LearningRateScheduler, ModelCheckpoint
from keras import optimizers
import numpy as np
import csv
import matplotlib.pyplot as plt
import os
import shutil
from tqdm import tqdm
np.set_printoptions(threshold=np.inf)


def init():
    name = "../Log/csv/log.csv"
    data = np.loadtxt(name, delimiter=",")
    return data, np.eye(2)

def init_vec():
    data = []
    teacher = np.eye(2)
    name = "../Log/csv/log_vec.csv"
    with open(name) as f:
        data_raw = f.readlines()
    k = 0
    for line in tqdm(data_raw):
        add_data = list(map(int,line.split(",")))
        num = [add_data[x] for x in range(len(add_data)) if x%2 == 0]
        num1 = [add_data[x] for x in range(len(add_data)) if x % 2 == 1]
        add_data = []
        for i in range(len(num)):
            for t in range(num1[i]):
                add_data.append(num[i])
        data.append(add_data)
    return data,teacher

def moredata(data, teacher):
    addData = np.copy(data)
    a, b = -1.0, 1.0
    for i in tqdm(range(100)):
        addData[0][0] += ((b - a) * np.random.rand() + a)/100
        addData[0][1] += ((b - a) * np.random.rand() + a)/100
        addData[1][0] += ((b - a) * np.random.rand() + a)/100
        addData[1][1] += ((b - a) * np.random.rand() + a)/100
        data = np.append(data, addData[0].reshape(1, 48), axis=0)
        data = np.append(data, addData[1].reshape(1, 48), axis=0)
        teacher = np.append(teacher, np.copy(teacher)[0].reshape(1, 2), axis=0)
        teacher = np.append(teacher, np.copy(teacher)[1].reshape(1, 2), axis=0)
    return data, teacher


def makemoredata(data, teacher):
    a, b = -1.0, 1.0
    for i in tqdm(range(100)):
        addData = np.copy(data)
        for x in range(0, len(addData[len(addData)-1]), 3):
            if addData[len(addData)-1][x] + addData[len(addData)-1][x+1] != 0.0:
                addData[len(addData)-1][x] += ((b - a)* np.random.rand() + a)/100
                addData[len(addData)-1][x+1] += ((b - a)* np.random.rand() + a)/100
        data = np.append(data, addData[len(addData)-1].reshape(1, 48), axis=0)
        teacher = np.append(teacher, np.copy(teacher)[len(
            teacher)-1].reshape(1, len(np.copy(teacher)[len(teacher)-1])), axis=0)
    return data, teacher

def network(data, teacher):
    es_cb = EarlyStopping(monitor='loss', patience=0, verbose=1, mode='auto')
    fliep = "./tensorlog/weights.hdf5"
    cp_cb = ModelCheckpoint(filepath=fliep, monitor="loss",verbose=1, save_best_only=True)
    newdata = np.copy(data[[0, 1]])
    answerdata = newdata
    newdata, teacher = moredata(newdata, teacher)
    layer = [Dense(1000, input_dim=561), Dense(2000), Activation("relu"), Dense(1000), Activation("relu"), Dense(2), Activation("softmax")]
    NN = Sequential()
    for i in layer:
        NN.add(i)
    NN.compile(optimizer="adam", loss="categorical_crossentropy",metrics=['accuracy'])
    NN.fit(newdata, teacher, epochs=10, callbacks=[es_cb, cp_cb], shuffle=True, validation_split=0.1)
    #for i in range(2,len(data)-1):
    for i in range(2, 100):
        testdata = NN.predict(data[0:i+1])
        print(testdata[testdata.shape[0]-1]
              [testdata[testdata.shape[0]-1].argmax()])
        if testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()] < 0.95:
            NN.pop()
            newdata = np.append(
                newdata, data[[testdata.shape[0]-1]].reshape(1, 48), axis=0)
            answerdata = np.append(
                answerdata, data[[testdata.shape[0]-1]].reshape(1, 48), axis=0)
            addDense1 = Dense(50)
            addDense4 = Dense(teacher.shape[1]+1)
            NN.add(addDense1)
            NN.add(Activation("sigmoid"))
            NN.add(addDense4)
            NN.add(Activation("softmax"))
            NN.compile(optimizer="adam",
                       loss="categorical_crossentropy", metrics=['accuracy'])
            teacher = teacher.T
            array = np.zeros((1, teacher.shape[1]))
            teacher = np.r_[teacher, array]
            teacher = teacher.T
            array2 = np.zeros((1, teacher.shape[1]))
            array2[0][array2.shape[1]-1] = 1
            teacher = np.r_[teacher, array2]
            newdata, teacher = makemoredata(newdata, teacher)
            NN.fit(newdata, teacher, epochs=100 +
                   int(pow(testdata.shape[1], 1.5)), callbacks=[cp_cb], validation_split=0.1)
        else:
            addData = np.zeros((1, testdata.shape[1]))
            addData[0][testdata[testdata.shape[0]-1].argmax()] = 1
            newdata = np.append(
                newdata, data[[testdata.shape[0]-1]].reshape(1, 48), axis=0)
            teacher = np.r_[teacher, addData]
        #NN = getNewestModel(NN, "./tensorlog/")
    testdata = NN.predict(data)
    print(testdata)
    ch = []
    for i in testdata:
        ch.append(np.argmax(i))
    with open("out.csv", "w+") as f:
        writer = csv.writer(f, lineterminator='\n')  # 改行コード（\n）を指定しておく
        writer.writerow(ch)     # list（1次元配列）の場合
    np.savetxt("answer.csv", answerdata, delimiter=",")
    NN.save("model_test.h5")
    NN.summary()

def network_vec(data,teacher):
    dropout = 0.3
    es_cb = EarlyStopping(monitor='val_loss', patience=0, verbose=1, mode='auto')
    newdata = np.array(data[0:2])
    answerdata = newdata
    layer = [Dense(1000, input_dim=561), Dropout(dropout), Dense(2000), Activation("relu"), Dropout(dropout), Dense(1000), Activation("relu"), Dropout(dropout), Dense(500), Activation("relu"), Dropout(dropout), Dense(2), Activation("softmax")]
    NN = Sequential()
    for i in layer:
        NN.add(i)
    NN.compile(optimizer="adam", loss="categorical_crossentropy",metrics=['accuracy'])
    NN.fit(newdata, teacher, epochs=150, callbacks=[es_cb], shuffle=True, validation_split=0.1)
    for i in range(2, 100):
        testdata = NN.predict(np.array(data[0:i+1]))
        print(testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()])
        if testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()] < 0.999:
            NN.pop()
            NN.pop()
            newdata = np.append(newdata, np.array(data[testdata.shape[0]-1]).reshape(1, 561), axis=0)
            answerdata = np.append(answerdata, np.array(data[testdata.shape[0]-1]).reshape(1, 561), axis=0)
            addDense1 = Dense(1000)
            addDense2 = Dense(500)
            addDense4 = Dense(teacher.shape[1]+1)
            NN.add(addDense1)
            NN.add(Activation("relu"))
            NN.add(Dropout(dropout))
            NN.add(addDense2)
            NN.add(Activation("relu"))
            NN.add(Dropout(dropout))
            NN.add(addDense4)
            NN.add(Activation("softmax"))
            NN.compile(optimizer="adam", loss="categorical_crossentropy", metrics=['accuracy'])
            teacher = teacher.T
            array = np.zeros((1, teacher.shape[1]))
            teacher = np.r_[teacher, array]
            teacher = teacher.T
            array2 = np.zeros((1, teacher.shape[1]))
            array2[0][array2.shape[1]-1] = 1
            teacher = np.r_[teacher, array2]
            NN.fit(newdata, teacher, epochs=150 +int(pow(testdata.shape[1], 1.5)), callbacks=[es_cb])
        else:
            addData = np.zeros((1, testdata.shape[1]))
            addData[0][testdata[testdata.shape[0]-1].argmax()] = 1
            newdata = np.append(newdata, np.array(data[testdata.shape[0]-1]).reshape(1, 561), axis=0)
            teacher = np.r_[teacher, addData]
        #NN = getNewestModel(NN, "./tensorlog/")
    testdata = NN.predict(np.array(data))

def getNewestModel(model, dirname):
    from glob import glob
    target = os.path.join(dirname, '*')
    files = [(f, os.path.getmtime(f)) for f in glob(target)]
    if len(files) == 0:
        return model
    else:
        newestModel = sorted(files, key=lambda files: files[1])[-1]
        model.load_weights(newestModel[0])
        return model


def main():
    data, teacher = init()
    network(data, teacher)
    """
    data, teacher = init_vec()
    network_vec(data,teacher)
    """

if __name__ == '__main__':
    main()
