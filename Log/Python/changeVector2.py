import csv
import numpy as np
from tqdm import tqdm
import math

def getAngle(x,y):
    angle = float(math.atan2(y-4.88, x-2.375))/float(math.pi*180.0)
    if angle<0.0:
        angle+=360.0
    angle=angle*(angle_num/360)
    return int(angle)

def getDist(x,y):
    d = math.sqrt( (x-2.375)*(x-2.375) + (y-4.88)*(y-4.88))
    return d/(dist_len/2.0)

def WriteFile(vec):
    with open('../csv/log_vec.csv', 'a') as f:
        writer = csv.writer(f, lineterminator='\n') # 改行コード（\n）を指定しておく
        writer.writerow(vec)     # list（1次元配列）の場合


with open("../csv/log.csv","r") as f:
    reader = csv.reader(f)
    angle_num=4
    dist_len=0.10
    dist_num=round(7/dist_len)
    j=0
    for row in tqdm(reader):#各行を取ってくる(32カラム)
        vec=["0"]*(angle_num*int(dist_num)*2+1)
        # if j<35000:#とりあえず35000行分
        for i in range(16):
            if float(row[0+i*2])>0 and float(row[1+i*2])>0:
                a =    getAngle( float(row[0+i*2]), float(row[1+i*2]) )
                d = int(getDist( float(row[0+i*2]), float(row[1+i*2]) ))
                if a!=0:
                    try:
                        vec[a*d*2+i%2]=str(1)
                    except:
                        print(a,d)
                        exit
                else:
                    vec[d*2+i%2]=str(1)
        ans=[]
        num=0
        while num < len(vec)-1:
            exist=vec[num]
            count=1
            while num+count<len(vec) and int(exist)==int(vec[num+count]):
                count+=1
            num = num+count
            ans.append(exist)
            ans.append(count)
        j+=1
        WriteFile(ans)
