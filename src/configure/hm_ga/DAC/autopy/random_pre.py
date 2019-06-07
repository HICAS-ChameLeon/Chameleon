#!usr/bin/env python
# -*- coding:utf-8 _*-

from rpy2.robjects import r
import random
import time

from collections import Sequence
from itertools import repeat
from HG_autoDAC import enCodein,deCodein,preByhm,selectPop,writeConf





def random_predic(indivList,time_conf_dasize):

    # print("evalute start...")
    datasize = 30.59316  # G
    deList = deCodein(indivList)

    pre_res = preByhm(deList,datasize)

    tmp = []
    tmp.append(pre_res[0][0])
    tmp = tmp + deList

    tmp.append(datasize)
    time_conf_dasize.append(tmp)

    return

def generateList():
    inList = []
    for i in range(34):
        inList.append(random.uniform(0,1))
    return inList

def main():
    t1 = time.clock()
    popSize = 10000     # the size of population
    time_conf_dasize = []

    for i in range(popSize):
        inList = generateList()
        random_predic(inList,time_conf_dasize)

    time_conf_dasize.sort(key=lambda x:x[0])
    random_file = open("D:/DACtest/random_tcd.txt","w")

    for i in range(len(time_conf_dasize)):
        # print(i)
        # print(len(time_conf_dasize[i]))
        # print(time_conf_dasize[i])

        tmp = ""
        tmp = tmp + str(time_conf_dasize[i])
        random_file.writelines(tmp)
        random_file.write("\n")

    random_file.close()

    t2 = time.clock()

    print("cost time :",t2-t1)

main()



