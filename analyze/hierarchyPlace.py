import os,csv,copy,shutil,concurrent.futures,numpy as np,math
r_li = []
a_li = []
c_li = []
node_li = []
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

#データ読み込み
def init():
  data = np.loadtxt("../Log/csv/log.csv",delimiter=",")
  return data

#極座標に変換
def plot(data):
  for i in data:
    for t in range(0,len(i),3):
      if i[t]+i[t+1] != 0:
        r = i[t]
        a = i[t+1]
        r_li.append(r)
        a_li.append(a)
        c_li.append(i[t+2])



#閾値の設定
def thresshold(x):
  return 8 ** x;

#木構造の生成
def maketree(a,r, node):
  if node.haschiled:
    if node.child1.minA <= a and node.child1.maxA > a and node.child1.minR <= r and node.child1.maxR > r:
      maketree(a,r,node.child1)
    elif node.child2.minA <= a and node.child2.maxA > a and node.child2.minR <= r and node.child2.maxR > r:
      maketree(a,r,node.child2)
    elif node.child3.minA <= a and node.child3.maxA > a and node.child3.minR <= r and node.child3.maxR > r:
      maketree(a,r,node.child3)
    else:
      maketree(a,r,node.child4)
  else:
    node.count += 1
    if node.count > thresshold(node.rank):
      node.createchildren()
    return

#木構造から検索
def search(a,r,node):
  if node.haschiled:
    if node.child1.minA <= a and node.child1.maxA > a and node.child1.minR <= r and node.child1.maxR > r:
      tmp = search(a, r, node.child1)
    elif node.child2.minA <= a and node.child2.maxA > a and node.child2.minR <= r and node.child2.maxR > r:
      tmp = search(a, r, node.child2)
    elif node.child3.minA <= a and node.child3.maxA > a and node.child3.minR <= r and node.child3.maxR > r:
      tmp = search(a, r, node.child3)
    else:
      tmp = search(a, r, node.child4)
  else:
    return node
  return tmp
        

def main():
  data = init()
  plot(data)
  root = Node(0,2.375*2,0,11,0)
  for i,_ in enumerate(r_li):
    maketree(r_li[i],a_li[i],root)
  NoHasChildList = [All_Node[i] for i in range(len(All_Node)) if All_Node[i].haschiled == False]
  binaridata = np.zeros((len(NoHasChildList)*2,len(data))).T
  #binaridata = np.zeros((len(NoHasChildList)*2,100)).T
  """
  for i,minidata in enumerate(data):
    for t in range(0, len(minidata), 3):
      if minidata[t]+minidata[t+1] != 0:
        r = math.atan2(minidata[t+1]-4.88, minidata[t]-2.375)/math.pi * 180
        a = math.sqrt((minidata[t+1]-4.88) ** 2 + (minidata[t]-2.375) ** 2)
        k = NoHasChildList.index(search(r, a, root))
        if(minidata[t+2]==0):
          binaridata[i][k*2] = 1
        else:
          binaridata[i][k*2+1] = 1
        """
  #np.savetxt("out1.csv",binaridata,fmt="%.2f")
  A1 = [NoHasChildList[i].minA for i in range(len(NoHasChildList)) if NoHasChildList[i].haschiled == False]
  A2 = [NoHasChildList[i].maxA for i in range(len(NoHasChildList)) if NoHasChildList[i].haschiled == False]
  R1 = [NoHasChildList[i].minR for i in range(len(NoHasChildList)) if NoHasChildList[i].haschiled == False]
  R2 = [NoHasChildList[i].maxR for i in range(len(NoHasChildList)) if NoHasChildList[i].haschiled == False]
  RANK = [NoHasChildList[i].rank for i in range(len(NoHasChildList)) if NoHasChildList[i].haschiled == False]


  print(len(A1))
  with open("out.csv","w") as f:
    writer = csv.writer(f,lineterminator='\n')
    writer.writerow(A1)
    writer.writerow(A2)
    writer.writerow(R1)
    writer.writerow(R2)
    writer.writerow(RANK)

if __name__ == '__main__':
    main()
