from rpy2.robjects import r
import random
from deap import tools
from deap import base, creator

# from HG_autoDAC import enCodein,deCodein,preByhm

def en_de_preTest():
    # inList = [37,11,36285,1865,92,
    #           1,107,876,1,0,1,30,89,1,123,
    #           124,0,1,0.8482439490352061,
    #           0.6382083374084615,0,784,68,
    #           23,33,448,331,8,15,1,89,
    #           3.489364557076994,0.07325905005015731,8]

    inList = [4,2,34494,33769,126,1,63,554,1,0,2,75,92,1,126,60,0,1,
              0.6232749531841193,0.7815248776358177,0,111,7,27,10,
              345,461,10,30,1,95,1.5166601701058977,0.21976525232978605,5]



    enList = enCodein(inList)
    deList = deCodein(enList)
    pre_res = preByhm(deList,50)
    print(type(pre_res[0][0]))
    print(type(123.4))
    print(pre_res)


    # print(len(inList))
    # print(len(deList))
    # print(len(enList))
    # print(inList)
    # print(deList)
    # print(enList)
    return

# en_de_preTest()

def strTofloat():
    tmp = "0.8107983888360739"
    tmpstr = "23"

    tmpf = float(tmp)
    tmpi = float(tmpstr)

    print(tmpf)
    print(tmpi)

    print((type(tmpf)))
    print((type(tmpi)))

    return

# strTofloat()



def makeResult(total):

    # read execution time from hibench.report
    benFile = open("D:/dacdata/time.txt","r")
    benFile.readline()  # skip the first line

    timeVec =[]   # store execution time
    dsizeVec = []  # stort input datasize


    for i in range(total):
        line = benFile.readline()
        line = ','.join((line.strip("\n")).split())    #replace "," with consecutive spaces
        word = list(line.split(","))
        # print(word)
        timeVec.append(word[-3])
        dsizeVec.append(word[-4])


    # read config from confVec.txt (vector)
    confVec = []
    confVecFile = open("D:/dacdata/err_timeConfVec.txt","r")
    for i in range(total):
        line = confVecFile.readline()
        line = line.strip("\n")
        line = line.strip(",")
        confVec.append(line.split(","))



    # combine config vector and execution time  and save comVec in file(time_conf.txt)
    comVecFile = open("D:/dacdata/err_time_conf_dsize.txt","w")
    comVec = []
    for i in range(total):
        tmpVec = []
        tailVec = []
        tmpVec.append(timeVec[i])
        tailVec.append(dsizeVec[i])

        # tmpVec = tmpVec + confVec[i]
        # tmpVec = tmpVec + confVec[i] + tailVec
        tmpVec = confVec[i] + tailVec
        # print(len(tmpVec))

        pstr = ""
        for tmp in range(len(tmpVec)):
            # pstr = pstr + str(tmpVec[tmp]) + ","
            if tmp == (len(tmpVec) - 1):
                pstr = pstr + str(tmpVec[tmp])
            else:
                pstr = pstr + str(tmpVec[tmp]) + ","

        if i < (total - 1):
            pstr = pstr + "\n"
        # pstr = pstr + "\n"

       # print(tmpVec)
       # print(pstr)
        comVecFile.write(pstr)

        comVec.append(tmpVec)



    # for tmp in range(total):
    #     # print(confVec[tmp])
    #     print(comVec[tmp])
    #     print("\n")
    #
    # print(confVec)
    # print(timeVec)

    benFile.close()
    confVecFile.close()
    comVecFile.close()


    return comVec

def addSizeToVector():
    r_ctdFile = open("D:/dacdata/err_timeConfVec.txt","r")
    w_ctdFile = open("D:/dacdata/err_time_conf_dsize.txt","w")

    linList = r_ctdFile.readlines()
    num = len(linList)
    print(num)
    # del(linList[-1])

    for i in range(num):
        if i < num -1:    # handle the last line ("\n")
            tmpStr = linList[i].strip("\n") + str(32849151872) + "\n"
        else:
            tmpStr = linList[i].strip("\n") + str(32849151872)
        w_ctdFile.write(tmpStr)

    r_ctdFile.close()
    w_ctdFile.close()

    return

# addSizeToVector()

#!/usr/bin/python3
import  os
def autoRun():
    command = "./run.sh"
    num = 3
    total = num
    suc = 0
    fail = 0
    for i in range(num):
        print("finished = ",i)
        print("suc = ",suc)
        print("fail = ",fail)
        result = os.system(command)
        if result == 0:
            suc += 1
        else:
            fail += 1

    print("total = ", total)
    print("suc = ", suc)
    print("fail = ", fail)
    return

# autoRun()
# print((2*3))

def test():
    i = 0
    flag = False
    for i in range(5):
        if i > 2:
            flag = True
            break
        print(i)

    if not (not flag):
        print("true",i)
    else:
        print("flase",i)

test()
# randomly generate 10000 config to predict

