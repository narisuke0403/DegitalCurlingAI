import glob
import os
#ファイルの場所を指定
def main():
  path = "../Log"
  output_path = "../Log_txt/"
  file = glob.glob(path+"*.dcl")
  t = len(file)
  for i in file:
    os.rename(i,output_path+str(t)+".txt")
    t+=1