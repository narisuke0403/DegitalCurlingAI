from keras.models import Model,Sequential,model_from_json
from keras.layers import Dense,Activation,Dropout,BatchNormalization
from keras.callbacks import EarlyStopping,LearningRateScheduler,ModelCheckpoint
from keras.layers.advanced_activations import ELU
from keras import optimizers
import numpy as np
import os
import shutil
import concurrent.futures
from sklearn.cluster import KMeans
import csv
import copy
np.set_printoptions(threshold=np.inf)

data = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]

def moredata(d, teacher):
  addData = np.copy(d)
  a, b = -1.0, 1.0
  for i in range(1000):
    addData[0][0] += ((b - a) * np.random.rand() + a)/100
    addData[0][1] += ((b - a) * np.random.rand() + a)/100
    addData[1][0] += ((b - a) * np.random.rand() + a)/100
    addData[1][1] += ((b - a) * np.random.rand() + a)/100
    d = np.append(d, addData[0].reshape(1, 3), axis=0)
    d = np.append(d, addData[1].reshape(1, 3), axis=0)
    teacher = np.append(teacher, np.copy(teacher)[1].reshape(1, 2), axis=0)
    teacher = np.append(teacher, np.copy(teacher)[0].reshape(1, 2), axis=0)
  return d, teacher


def makemoredata(d, teacher):
  a, b = -1.0, 1.0
  for i in range(1000):
    addData = np.copy(d)
    for x in range(0, len(addData[len(addData)-1]), 3):
      addData[len(addData)-1][x] += ((b - a)* np.random.rand() + a)/100
      addData[len(addData)-1][x+1] += ((b - a)* np.random.rand() + a)/100
    d = np.append(d, addData[len(addData)-1].reshape(1, 3), axis=0)
    teacher = np.append(teacher, np.copy(teacher)[len(teacher)-1].reshape(1, len(np.copy(teacher)[len(teacher)-1])), axis=0)
  return d, teacher

def init(i):
  path = "../Log/csv/csv_parse/"
  name = "log_"+str(i)+".csv"
  data[i] = np.loadtxt(path+name,delimiter=",")

def network(stone_num):
  act = "relu"
  init(stone_num)
  dropout = 0.3
  newdata = data[stone_num]
  es_cb = EarlyStopping(monitor='loss',patience=100, verbose=0, mode='auto')
  traindata = np.copy(newdata[0:2])
  teacher = np.eye(2)
  #traindata,teacher = moredata(traindata,teacher)
  answerdata = traindata
  layer = [Dense(100, input_dim=8 + 8 * stone_num),Activation("sigmoid"),Dropout(dropout), Dense(100), Activation(act), Dropout(dropout), Dense(100), Activation(act), Dropout(dropout), Dense(2), Activation("softmax")]
  NN = Sequential()
  for i in layer:
    NN.add(i)
  NN.compile(optimizer="adam",loss="categorical_crossentropy", metrics=['accuracy'])
  NN.fit(traindata,teacher,epochs=500,callbacks=[es_cb],shuffle=True)
  limit = 0.999
  #for i in range(2,len(newdata)-1):
  for i in range(2,4):
    testdata = NN.predict(newdata[0:i+1])
    print(testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()])
    if testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()] < limit:
      limit *= 0.95
      NN.pop()
      NN.pop()
      traindata = np.append(traindata, newdata[[testdata.shape[0]-1]].reshape(1, 8+ stone_num*8), axis=0)
      answerdata = np.append(answerdata, newdata[[testdata.shape[0]-1]].reshape(1, 8 + stone_num*8), axis=0)
      addDense1 = Dense(100)
      addDesen2 = Dense(100)
      addDense3 = Dense(teacher.shape[1]+1)
      NN.add(addDense1)
      NN.add(Activation(act))
      NN.add(Dropout(dropout))
      NN.add(addDesen2)
      NN.add(Activation(act))
      NN.add(Dropout(dropout))
      NN.add(addDense3)
      NN.add(Activation("softmax"))
      NN.compile(optimizer="adam", loss="categorical_crossentropy",metrics=['accuracy'])
      teacher = teacher.T
      array = np.zeros((1,teacher.shape[1]))
      teacher = np.r_[teacher, array]
      teacher = teacher.T
      array2 = np.zeros((1,teacher.shape[1]))
      array2[0][array2.shape[1]-1] = 1
      teacher = np.r_[teacher, array2]
      #traindata,teacher = makemoredata(traindata,teacher)
      if len(traindata) > 100:
        NN.fit(traindata, teacher, epochs=500 +int(pow(testdata.shape[1], 1.5)),validation_split=0.1)
      else:
        NN.fit(traindata, teacher, epochs=500 + int(pow(testdata.shape[1], 1.5)))
    else:
      addData = np.zeros((1,testdata.shape[1]))
      addData[0][testdata[testdata.shape[0]-1].argmax()]=1
      traindata = np.append(traindata, newdata[[testdata.shape[0]-1]].reshape(1, 8 + stone_num*8), axis=0)
      teacher=np.r_[teacher,addData]
  testdata = NN.predict(traindata)
  NN_json_str = NN.to_json()
  with open("model"+str(stone_num)+".json","w") as f:
    f.write(NN_json_str)
  NN.save_weights("model_weight"+str(stone_num)+".h5")
  ch = []
  for i in testdata:
    ch.append(np.argmax(i))
  with open("out"+str(stone_num)+".csv", "w+") as f:
    writer = csv.writer(f, lineterminator='\n')  # 改行コード（\n）を指定しておく
    writer.writerow(ch)     # list（1次元配列）の場合
  np.savetxt("answer"+str(stone_num)+".csv",answerdata,delimiter=",")


def network_use_classtaring(stone_num):
  teacher = classtaring(stone_num)
  newdata = data[stone_num]
  es_cb = EarlyStopping(monitor='loss',patience=0, verbose=0, mode='auto')
  traindata = newdata
  answerdata = []
  layer = [Dense(500, input_dim=8 + 8 * stone_num), BatchNormalization(), Dropout(0.2), Dense(1000), Activation("relu"), Dropout(0.2), Dense(50), Activation("relu"), Dropout(0.2), Dense(len(teacher)), Activation("softmax")]
  NN = Sequential()
  for i in layer:
    NN.add(i)
  NN.summary()
  NN.compile(optimizer="adam", loss="sparse_categorical_crossentropy", metrics=['accuracy'])
  NN.fit(traindata,teacher,epochs=150,callbacks=[es_cb],shuffle=True,validation_split=0.1)
  """
  for i in range(2,len(newdata)-1):
    testdata = NN.predict(newdata[0:i+1])
    print(testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()])
    if testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()] < 0.999:
      NN.pop()
      NN.pop()
      traindata = np.append(traindata, newdata[[testdata.shape[0]-1]].reshape(1, 3 + stone_num*3), axis=0)
      answerdata = np.append(answerdata, newdata[[testdata.shape[0]-1]].reshape(1, 3 + stone_num*3), axis=0)
      addDense1 = Dense(100)
      addDense2 = Dense(teacher.shape[1]+1)
      NN.add(addDense1)
      NN.add(Dropout(0.2))
      NN.add(addDense2)
      NN.add(Activation("softmax"))
      NN.compile(optimizer="adam", loss="categorical_crossentropy",metrics=['accuracy'])
      teacher = teacher.T
      array = np.zeros((1,teacher.shape[1]))
      teacher = np.r_[teacher, array]
      teacher = teacher.T
      array2 = np.zeros((1,teacher.shape[1]))
      array2[0][array2.shape[1]-1] = 1
      teacher = np.r_[teacher, array2]
      NN.fit(traindata, teacher, epochs=150 +int(pow(testdata.shape[1], 1.5)),callbacks=[es_cb])
    else:
      addData = np.zeros((1,testdata.shape[1]))
      addData[0][testdata[testdata.shape[0]-1].argmax()]=1
      traindata = np.append(traindata, newdata[[testdata.shape[0]-1]].reshape(1, 3 + stone_num*3), axis=0)
      teacher=np.r_[teacher,addData]
  """
  testdata = NN.predict(newdata)
  print(testdata)
  """
  NN_json_str = NN.to_json()
  with open("model"+str(stone_num)+".json","w") as f:
    f.write(NN_json_str)
  NN.save_weights("model_weight"+str(stone_num)+".h5")
  """

def classtaring(i):
  init(i)
  cust_df = data[i]
  teacher_num = (i+1)*10
  pred = KMeans(n_clusters=teacher_num).fit_predict(cust_df)
  return pred


traindatas = []
answerdatas = []
chs = []
def network_new(_data):
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
    print(testdata)
    testdata = NN.predict(_tempdata)
    ch = []
    for i in range(len(testdata)):
      ch.append(testdata[i].argmax())
    chs.append(ch)
    print(len(tempdata))
    network_new(tempdata)
  
def write_answer(t):
  for i in range(len(t)):
    np.savetxt("new/data_"+str(i)+".csv",t[i],delimiter=",",fmt="%0.5f")
    np.savetxt("new/answernum_"+str(i)+".csv",answerdatas[i],delimiter=",",fmt="%0.5f")
    with open("new/out_"+str(i)+".csv", "w+") as f:
      writer = csv.writer(f, lineterminator='\n')  # 改行コード（\n）を指定しておく
      writer.writerow(chs[i])     # list（1次元配列）の場合

if __name__ == "__main__":
  """
  executor = concurrent.futures.ThreadPoolExecutor(max_workers=3)
  for i in range(15):
    executor.submit(network,i)
  
  for i in range(15):
    network(i)
  """
  num = 0
  init(num)
  network_new(data[num])
  write_answer(traindatas)