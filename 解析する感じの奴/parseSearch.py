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
from tqdm import tqdm
import csv
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
  act = "sigmoid"
  init(stone_num)
  dropout = 0.3
  newdata = data[stone_num]
  es_cb = EarlyStopping(monitor='loss',patience=100, verbose=0, mode='auto')
  traindata = np.copy(newdata[0:2])
  teacher = np.eye(2)
  #traindata,teacher = moredata(traindata,teacher)
  answerdata = traindata
  layer = [Dense(500, input_dim=8 + 8 * stone_num),Activation("linear"),Dropout(dropout), Dense(1000), Activation(act), Dropout(dropout), Dense(500), Activation(act), Dropout(dropout), Dense(2), Activation("softmax")]
  NN = Sequential()
  for i in layer:
    NN.add(i)
  NN.compile(optimizer="adam",loss="categorical_crossentropy", metrics=['accuracy'])
  NN.fit(traindata,teacher,epochs=150,callbacks=[es_cb],shuffle=True)
  limit = 0.999
  #for i in range(2,len(newdata)-1):
  for i in range(2,100):
    testdata = NN.predict(newdata[0:i+1])
    for t in range(len(testdata)-1):
      if teacher[t].argmax() != testdata[t].argmax():
        print("違う")
        print(t)
        print(teacher[t].argmax())
        print(testdata[t].argmax())
        print(testdata[t][testdata[t].argmax()])
        if teacher[t].argmax() < testdata[t].argmax():
          teacher[t][teacher[t].argmax()] = 0
          teacher[t][testdata[t].argmax()] = 1
        
    print(testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()])
    if testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()] < limit:
      limit *= 0.95
      NN.pop()
      NN.pop()
      traindata = np.append(traindata, newdata[[testdata.shape[0]-1]].reshape(1, 8+ stone_num*8), axis=0)
      answerdata = np.append(answerdata, newdata[[testdata.shape[0]-1]].reshape(1, 8 + stone_num*8), axis=0)
      addDense1 = Dense(1000)
      addDesen2 = Dense(500)
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
        NN.fit(traindata, teacher, epochs=150 +int(pow(testdata.shape[1], 1.5)),validation_split=0.1)
      else:
        NN.fit(traindata, teacher, epochs=150 + int(pow(testdata.shape[1], 1.5)))
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

def zscore(x, axis = None):
  a_x = np.array([x[i][0] for i in range(len(x))])
  a_y = np.array([x[i][1] for i in range(len(x))])
  a_c = np.array([x[i][2] for i in range(len(x))])
  a = [a_x,a_y]
  for i in a:
    xmean = i.mean(axis=axis, keepdims=True)
    xstd = np.std(i, axis=axis, keepdims=True)
    zscore = (i-xmean)/xstd
    if np.allclose(i,a_x):
      a_x = zscore
    else:
      a_y = zscore
  j = np.zeros((len(a_c),3))
  for i in range(len(a_c)):
    add_j = np.array([a_x[i],a_y[i],a_c[i]])
    j[i] = add_j
  return j

def network_zscore(stone_num):
  init(stone_num)
  data[stone_num] = zscore(data[stone_num])
  print(data[stone_num])
  dropout = 0.3
  newdata = data[stone_num]
  es_cb = EarlyStopping(monitor='loss',patience=0, verbose=0, mode='auto')
  traindata = newdata[0:2]
  print(traindata)
  teacher = np.eye(2)
  answerdata = traindata
  layer = [Dense(500, input_dim=3 + 3 * stone_num), Dropout(dropout), Dense(1000), Activation("sigmoid"), Dropout(dropout), Dense(50), Activation("sigmoid"), Dropout(dropout), Dense(len(teacher)), Activation("softmax")]
  NN = Sequential()
  for i in layer:
    NN.add(i)
  NN.summary()
  NN.compile(optimizer="adam", loss="categorical_crossentropy",metrics=['accuracy'])
  NN.fit(traindata,teacher,epochs=150,callbacks=[es_cb],shuffle=True)
  limit = 0.95
  #for i in tqdm(range(2,len(newdata)-1)):
  for i in range(2,200):
    testdata = NN.predict(newdata[0:i+1])
    print(testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()])
    if testdata[testdata.shape[0]-1][testdata[testdata.shape[0]-1].argmax()] < limit:
      NN.pop()
      NN.pop()
      traindata = np.append(traindata, newdata[[testdata.shape[0]-1]].reshape(1, 3 + stone_num*3), axis=0)
      answerdata = np.append(answerdata, newdata[[testdata.shape[0]-1]].reshape(1, 3 + stone_num*3), axis=0)
      addDense1 = Dense(100)
      addDesen2 = Dense(50)
      addDense3 = Dense(teacher.shape[1]+1)
      NN.add(addDense1)
      NN.add(Dropout(dropout))
      NN.add(addDesen2)
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
      NN.fit(traindata, teacher, epochs=150 + int(pow(testdata.shape[1], 1.5)))
    else:
      addData = np.zeros((1,testdata.shape[1]))
      addData[0][testdata[testdata.shape[0]-1].argmax()]=1
      traindata = np.append(traindata, newdata[[testdata.shape[0]-1]].reshape(1, 3 + stone_num*3), axis=0)
      teacher=np.r_[teacher,addData]
  testdata = NN.predict(newdata)
  print(testdata)
  NN_json_str = NN.to_json()
  with open("model"+str(stone_num)+".json","w") as f:
    f.write(NN_json_str)
  NN.save_weights("model_weight"+str(stone_num)+".h5")

def network_use_classtaring(stone_num):
  init(stone_num)
  newdata = data[stone_num]
  es_cb = EarlyStopping(monitor='loss',patience=0, verbose=0, mode='auto')
  traindata = newdata
  teacher = classtaring(stone_num)
  answerdata = traindata
  layer = [Dense(500, input_dim=3 + 3 * stone_num), BatchNormalization(), Dropout(0.2), Dense(1000), Activation("sigmoid"), Dropout(0.2), Dense(50), Activation("sigmoid"), Dropout(0.2), Dense(len(teacher)), Activation("softmax")]
  NN = Sequential()
  for i in layer:
    NN.add(i)
  NN.summary()
  NN.compile(optimizer="adam", loss="categorical_crossentropy",metrics=['accuracy'])
  NN.fit(traindata,teacher,epochs=150,callbacks=[es_cb],shuffle=True)
  
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
  
  testdata = NN.predict(newdata)
  print(testdata)
  NN_json_str = NN.to_json()
  with open("model"+str(stone_num)+".json","w") as f:
    f.write(NN_json_str)
  NN.save_weights("model_weight"+str(stone_num)+".h5")

def classtaring(i):
  cust_df = data[i]
  teacher_num = 100
  teacher = np.zeros((teacher_num,len(cust_df)))
  pred = KMeans(n_clusters=teacher_num).fit_predict(cust_df)
  for i in tqdm(range(len(pred))):
    teacher[i][pred[i]] = 1
  return teacher

if __name__ == "__main__":
  """
  executor = concurrent.futures.ThreadPoolExecutor(max_workers=3)
  for i in range(15):
    executor.submit(network,i)
  
  for i in range(15):
    network(i)
  """
  network(0)
