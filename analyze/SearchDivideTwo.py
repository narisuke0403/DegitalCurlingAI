from keras.models import Model,Sequential,model_from_json
from keras.layers import Dense,Activation,Dropout,BatchNormalization
from keras.callbacks import EarlyStopping,LearningRateScheduler,ModelCheckpoint
from keras import optimizers
import os,csv,copy,shutil,concurrent.futures,numpy as np

np.set_printoptions(threshold=np.inf)

data = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
traindatas = []
answerdatas = []
chs = []

def init(i):
  path = "../Log/csv/csv_parse/"
  name = "log_"+str(i)+".csv"
  data[i] = np.loadtxt(path+name,delimiter=",")

def networkDivideTwo(_data):
  act = "relu"
  newdata = _data
  dropout = 0.2
  es_cb = EarlyStopping(monitor='loss',patience=100, verbose=0, mode='auto')
  traindata = newdata[[int(np.random.randint(0,len(newdata))),int(np.random.randint(0,len(newdata)))]]
  answerdatas.append(traindata)
  teacher = np.eye(2)
  layer = [Dense(100, input_dim=len(_data[0])),Activation("linear"),Dropout(dropout), Dense(100), Activation(act), Dropout(dropout), Dense(100), Activation(act), Dropout(dropout), Dense(teacher.shape[1]), Activation("softmax")]
  NN = Sequential()
  for i in layer:
    NN.add(i)
  NN.compile(optimizer="adam", loss="categorical_crossentropy",metrics=['accuracy'])
  NN.fit(traindata,teacher,epochs=500,callbacks=[es_cb],shuffle=True)
  tempdata = []
  _tempdata = []
  testdata = NN.predict(newdata)
  for i in range(len(newdata)):
    if(testdata[i][testdata[i].argmax()] < 0.999):
      tempdata.append(newdata[i])
    else:
      _tempdata.append(newdata[i])
  if(len(tempdata) < 2):
    return
  else:
    tempdata = np.array(tempdata)
    _tempdata = np.array(_tempdata)
    traindatas.append(copy.deepcopy(_tempdata))
    testdata = NN.predict(_tempdata)
    ch = []
    for i in range(len(testdata)):
      ch.append(testdata[i].argmax())
    chs.append(ch)
    network_new(tempdata)

def main():
  num = 0
  init(num)
  networkDivideTwo(data[num])

if __name__ == '__main__':
    main()
    