import glob
data_all = []

def ReadFile(name):
    with open(name,'r') as f:
        i = 0
        for row in f:
            if list(row.strip())[0] == 'P':
                data_row = row.strip().split()
                data = data_row[1:len(data_row)]
                data_all.append(data)
            i+=1

def WriteFile():
    with open('../csv/log.csv','a') as f:
        for i in data_all:
            data_write = ""
            x = 0
            for t in i:
                print(i)
                print(t)
                if x%3==2:
                    t = int(t,10) * 200 - 100
                if x == len(i)-1:
                    data_write += str(t)
                else:
                    data_write += str(t) + ","
                x += 1
            f.write(data_write + "\n")

def main():
    filetxt=glob.glob("../Log_txt/*.txt")
    for k in filetxt:
        try:
            ReadFile(k)
        except :
            return
    WriteFile()
    

if __name__ == '__main__':
    main()
    