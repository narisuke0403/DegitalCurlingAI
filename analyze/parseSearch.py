from keras.models import Model,Sequential,model_from_json
from keras.layers import Dense,Activation,Dropout,BatchNormalization
from keras.callbacks import EarlyStopping,LearningRateScheduler,ModelCheckpoint
from keras import optimizers
import os,csv,copy,shutil,concurrent.futures,numpy as np

np.set_printoptions(threshold=np.inf)

data = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]

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

if __name__ == "__main__":
  """
  executor = concurrent.futures.ThreadPoolExecutor(max_workers=3)
  for i in range(15):
    executor.submit(network,i)
  
  for i in range(15):
    network(i)
  """
  num = 1
  network(stone_num)