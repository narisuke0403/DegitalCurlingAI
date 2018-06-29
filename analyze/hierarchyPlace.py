import os,csv,copy,shutil,concurrent.futures,numpy as np,math
x_li = []
y_li = []
All_Node_x = []
All_Node_y = []
x1 = []
x2 = []
y1 = []
y2 = []

class Node_x:
  def __init__(self,_min,_max,_rank):
    self.min = _min
    self.max = _max
    self.rank = _rank
    self.child1 = None
    self.child2 = None
    self.haschiled = False
    self.count = 0
  
  def createchildren(self):
    self.child1 = Node_x(self.min, self.min + (self.max-self.min)/2, self.rank+1)
    self.child2 = Node_x(self.min + (self.max-self.min)/2, self.max , self.rank+1)
    All_Node_x.append(self.child1)
    All_Node_x.append(self.child2)
    self.haschiled = True

class Node_y(Node_x):
  def __init__(self,_min,_max,_rank):
    super().__init__(_min,_max,_rank)

  def createchildren(self):
    self.child1 = Node_y(self.min, self.min + (self.max-self.min)/2, self.rank+1)
    self.child2 = Node_y(self.min + (self.max-self.min)/2, self.max , self.rank+1)
    All_Node_y.append(self.child1)
    All_Node_y.append(self.child2)
    self.haschiled = True

def init():
  data = np.loadtxt("../Log/csv/log.csv",delimiter=",")
  return data

def plot(data):
  for i in data:
    for t in range(0,len(i),3):
      if i[t]+i[t+1] != 0:
        r = math.atan2(i[t+1]-4.88,i[t]-2.375)/math.pi * 180
        l = math.sqrt((i[t+1]-4.88) ** 2 + (i[t]-2.375) ** 2)
        x_li.append(r)
        y_li.append(l)

def thresshold(x):
  return 3 ** x

def search(x, node):
  if node.haschiled:
    if node.child1.min <= x and node.child1.max > x:
      search(x, node.child1)
    else:
      search(x, node.child2)
  else:
    node.count += 1
    if node.count > thresshold(node.rank):
      node.createchildren()
    return
  
def main():
  plot(init())
  root_x = Node_x(0,360,0)
  root_y = Node_y(0,11,0)
  for i,_ in enumerate(x_li):
    search(x_li[i],root_x)
    search(y_li[i],root_y)
  x1 = [All_Node_x[i].min for i in range(len(All_Node_x)) if All_Node_x[i].haschiled == False]
  x2 = [All_Node_x[i].max for i in range(len(All_Node_x)) if All_Node_x[i].haschiled == False]
  y1 = [All_Node_y[i].min for i in range(len(All_Node_y)) if All_Node_y[i].haschiled == False]
  y2 = [All_Node_y[i].max for i in range(len(All_Node_y)) if All_Node_y[i].haschiled == False]
  with open("out.csv","w+") as f:
    writer = csv.writer(f,lineterminator="\n")
    writer.writerow(x1)
    writer.writerow(x2)
    writer.writerow(y1)
    writer.writerow(y2)
if __name__ == '__main__':
    main()