import numpy as np
import csv
import math

data = []
field_stone_info = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
def readFile():
  with open("../csv/log.csv","r") as f:
    render = csv.reader(f)
    for row in render:
      data.append(list(map(float,row)))
      
def writeFile():
  for i in range(len(field_stone_info)):
    name = "log_"+str(i)+".csv"
    np.savetxt("../csv/"+name, field_stone_info[i], fmt="%.8f",delimiter=",")

def analyze():
  s = 1
  for i in data:
    stone_num = 0
    stone_info = []
    for t in range(0,len(i),3):
      if i[t]+i[t+1] != 0:
        i[t] -= 2.375
        i[t] /= (2.375/2)
        i[t+1] -= 4.88
        i[t+1] /= (4.88/2)
        i[t+2] = i[t+2]*2 - 1
        stone_info.append(i[t])
        stone_info.append(i[t+1])
        stone_info.append(i[t+2])
        stone_info.append(i[t]**2)
        stone_info.append(i[t+1]**2)
        stone_info.append(i[t] * i[t+1])
        stone_info.append(math.sin(i[t]))
        stone_info.append(math.sin(i[t+1]))
        stone_num += 1
    if stone_num != 0:
      field_stone_info[stone_num-1].append(stone_info)
    s+=1
def main():
  readFile()
  analyze()
  writeFile()


if __name__ == '__main__':
    main()
    
