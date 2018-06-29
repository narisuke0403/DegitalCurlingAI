from keras.models import Model,Sequential,model_from_json
from keras.layers import Dense,Activation,Dropout,BatchNormalization
from keras.callbacks import EarlyStopping,LearningRateScheduler,ModelCheckpoint
from keras import optimizers
from sklearn.mixture import BayesianGaussianMixture
import os,csv,copy,shutil,concurrent.futures,numpy as np

np.set_printoptions(threshold=np.inf)

data = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]

def init(i):
  path = "../Log/csv/csv_parse/"
  name = "log_"+str(i)+".csv"
  data[i] = np.loadtxt(path+name,delimiter=",")

def network_use_clustering(stone_num):
  BATCH_DATA_NUM = 1000
  init(stone_num)
  newdata = data[stone_num]
  random_data = data[stone_num][np.random.choice(np.array(range(len(data[stone_num]))),BATCH_DATA_NUM)]
  teacher = clustering(random_data)
  es_cb = EarlyStopping(monitor='loss',patience=0, verbose=0, mode='auto')
  traindata = newdata
  answerdata = []
  layer = [Dense(500, input_dim=8 + 8 * stone_num), BatchNormalization(), Dropout(0.2), Dense(1000), Activation("relu"), Dropout(0.2), Dense(50), Activation("relu"), Dropout(0.2), Dense(len(teacher)), Activation("softmax")]
  NN = Sequential()
  for i in layer:
    NN.add(i)
  NN.summary()
  NN.compile(optimizer="adam", loss="categorical_crossentropy", metrics=['accuracy'])
  NN.fit(traindata,teacher,epochs=150,callbacks=[es_cb],shuffle=True,validation_split=0.1)
  testdata = NN.predict(newdata)
  print(testdata)
  NN_json_str = NN.to_json()
  with open("model"+str(stone_num)+".json","w") as f:
    f.write(NN_json_str)
  NN.save_weights("model_weight"+str(stone_num)+".h5")

def clustering(x):
  cust_df = np.copy(x)
  pred =BayesianGaussianMixture(n_components=len(cust_df)).fit(cust_df)
  pred = pred.predict(cust_df)
  max_number = np.argmax(pred)
  print(pred)
  print(pred[max_number])
  t = 0
  for i in range(1000):
    if pred.tolist().count(i) != 0:
      t += 1
  print(t)

def main():
  num = 0
  init(num)
  clustering(data[num])