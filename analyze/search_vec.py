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
    testdata = NN.predict(np.array(data))

def main():
    data, teacher = init_vec()
    network_vec(data,teacher)

if __name__ == '__main__':
    main()