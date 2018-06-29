import os,csv,copy,shutil,concurrent.futures,numpy as np,math
x_li = []
y_li = []
All_Node = []

class Node:
  def __init__(self,_minA,_maxA,_minR,_maxR,_rank):
    self.minA = _minA
    self.maxA = _maxA
    self.minR = _minR
    self.maxR = _maxR
    self.rank = _rank
    self.child1 = None
    self.child2 = None
    self.child3 = None
    self.child4 = None
    self.haschiled = False
    self.count = 0
  
  def createchildren(self):
    self.child3 = Node(self.minA, self.minA + (self.maxA-self.minA)/2,self.minR + (self.maxR-self.minR)/2,self.maxR,self.rank+1)
    self.child2 = Node(self.minA + (self.maxA-self.minA)/2, self.maxA ,self.minR,self.minR + (self.maxR-self.minR)/2, self.rank+1)
    self.child1 = Node(self.minA, self.minA + (self.maxA-self.minA)/2,self.minR,self.minR + (self.maxR-self.minR)/2, self.rank+1)
    self.child4 = Node(self.minA + (self.maxA-self.minA)/2, self.maxA ,self.minR + (self.maxR-self.minR)/2,self.maxR, self.rank+1)
    All_Node.append(self.child1)
    All_Node.append(self.child2)
    All_Node.append(self.child3)
    All_Node.append(self.child4)
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

def search(a,r, node):
  if node.haschiled:
    if node.child1.minA <= a and node.child1.maxA > a and node.child1.minR <= r and node.child1.maxR > r:
      search(a,r,node.child1)
    elif node.child2.minA <= a and node.child2.maxA > a and node.child2.minR <= r and node.child2.maxR > r:
      search(a,r,node.child2)
    elif node.child3.minA <= a and node.child3.maxA > a and node.child3.minR <= r and node.child3.maxR > r:
      search(a,r,node.child3)
    else:
      search(a,r,node.child4)
  else:
    node.count += 1
    if node.count > thresshold(node.rank):
      node.createchildren()
    return
  
def main():
  plot(init())
  root = Node(0,360,0,11,0)
  for i,_ in enumerate(x_li):
    search(x_li[i],y_li[i],root)
  A1 = [All_Node[i].minA for i in range(len(All_Node)) if All_Node[i].haschiled == False]
  A2 = [All_Node[i].maxA for i in range(len(All_Node)) if All_Node[i].haschiled == False]
  R1 = [All_Node[i].minR for i in range(len(All_Node)) if All_Node[i].haschiled == False]
  R2 = [All_Node[i].maxR for i in range(len(All_Node)) if All_Node[i].haschiled == False]

  with open("out.csv","w") as f:
    writer = csv.writer(f,lineterminator='\n')
    writer.writerow(A1)
    writer.writerow(A2)
    writer.writerow(R1)
    writer.writerow(R2)

if __name__ == '__main__':
    main()