#!/usr/bin/python3
import rpy2
import os, random
import rpy2.robjects as robjects
from rpy2.robjects.packages import importr

from rpy2.robjects import r
from deap import base, creator

# update time_conf to time_conf_dsize

def addSizeToVector():
    r_ctdFile = open("./time_conf_80_1184.txt","r")
    w_ctdFile = open("./time_conf_dsize_80_1184.txt","w")

    linList = r_ctdFile.readlines()
    num = len(linList)
    print(num)
    # del(linList[-1])

    for i in range(num):
        if i < num -1:    # handle the last line ("\n")
            tmpStr = linList[i].strip("\n") + str(80) + "\n"
        else:
            tmpStr = linList[i].strip("\n") + str(80)
        w_ctdFile.write(tmpStr)

    r_ctdFile.close()
    w_ctdFile.close()

    return

# addSizeToVector()
# Write the optimal value found by GA
#  to the spark configuration file

def writeConf(configVec):

    configList = []     #Storage profile

    # Writer history-server config (Not in the scope of parameter optimization)
    configList.append("spark.eventLog.enabled             true\n")
    configList.append("spark.history.fs.logDirectory      hdfs://zyx:9000/history\n")
    configList.append("spark.eventLog.dir                 hdfs://zyx:9000/history\n")
    configList.append("spark.yarn.historyServer.address   zyx:18080\n")
    configList.append("spark.eventLog.compress            true\n")


    # The num of executors

    # spark_executor_instances = random.randint(1,100)  #(1,60)
    spark_executor_instances = configVec[0]  #(1,60)
    configList.append("spark.executor.instances      " + str(spark_executor_instances) + "\n")


    # Application Propertie

    # spark_driver_cores = random.randint(1,20)  #(1,60)
    spark_driver_cores = configVec[1]  #(1,60)
    configList.append("spark.driver.cores         " + str(spark_driver_cores) + "\n")

    # spark_driver_memory = random.randint(1024,36864)
    spark_driver_memory = configVec[2]
    configList.append("spark.driver.memory        " + str(spark_driver_memory) + "m\n")

    # spark_executor_memory = random.randint(1024,36864)
    spark_executor_memory = configVec[3]
    configList.append("spark.executor.memory        " + str(spark_executor_memory) + "m\n")


    # Shuffle Behavior

    # spark_reducer_maxSizeInFlight = random.randint(2,128)
    spark_reducer_maxSizeInFlight = configVec[4]
    configList.append("spark.reducer.maxSizeInFlight          " + str(spark_reducer_maxSizeInFlight) + "m\n")


    if configVec[5] == 1:
        spark_shuffle_compress = True
    elif configVec[5] == 0:
        spark_shuffle_compress = False
    else:
        spark_shuffle_compress = "error"
    configList.append("spark.shuffle.compress        " + str(spark_shuffle_compress) + "\n")


    # spark_shuffle_file_buffer = random.randint(2,128)
    spark_shuffle_file_buffer = configVec[6]
    configList.append("spark.shuffle.file.buffer      " +  str(spark_shuffle_file_buffer) + "k\n")

    # spark_shuffle_sort_bypassMergeThreshold = random.randint(100,1000)
    spark_shuffle_sort_bypassMergeThreshold = configVec[7]
    configList.append("spark.shuffle.sort.bypassMergeThreshold  " + str(spark_shuffle_sort_bypassMergeThreshold) + "\n")

    if configVec[8] == 1:
        spark_shuffle_spill_compress = True
    elif configVec[8] == 0:
        spark_shuffle_spill_compress = False
    else:
        spark_shuffle_spill_compress = "error"
    configList.append("spark.shuffle.spill.compress      " + str(spark_shuffle_spill_compress) + "\n")


    # Compression and Serialization

    if configVec[9] == 1:
        spark_broadcast_compress = True
    elif configVec[9] == 0:
        spark_broadcast_compress = False
    else:
        spark_broadcast_compress = "error"
    configList.append("spark.broadcast.compress      " + str(spark_broadcast_compress) + "\n")


    if configVec[10] == 0:
        spark_io_compression_codec = "snappy"
    elif configVec[10] == 1:
        spark_io_compression_codec = "lz4"
    elif configVec[10] == 2:
        spark_io_compression_codec = "lzf"
    else:
        spark_io_compression_codec = "error"
    configList.append("spark.io.compression.codec      " + str(spark_io_compression_codec) + "\n")


    # spark_io_compression_snappy_blockSize = random.randint(2,128)
    spark_io_compression_snappy_blockSize = configVec[11]
    configList.append("spark.io.compression.snappy.blockSize  " + str(spark_io_compression_snappy_blockSize) + "k\n")

    # spark_io_compression_lz4_blockSize = random.randint(2,128)
    spark_io_compression_lz4_blockSize = configVec[12]
    configList.append("spark.io.compression.lz4.blockSize   " + str(spark_io_compression_lz4_blockSize) + "k\n")

    if configVec[13] == 1:
        spark_kryo_referenceTracking = True
    elif configVec[13] == 0:
        spark_kryo_referenceTracking = False
    else:
        spark_kryo_referenceTracking = "error"
    configList.append("spark.kryo.referenceTracking      " + str(spark_kryo_referenceTracking) + "\n")

    # spark_kryoserializer_buffer_max = random.randint(8,128)
    spark_kryoserializer_buffer_max = configVec[14]
    configList.append("spark.kryoserializer.buffer.max      " + str(spark_kryoserializer_buffer_max) + "m\n")

    # spark_kryoserializer_buffer = random.randint(2,128)
    spark_kryoserializer_buffer = configVec[15]
    configList.append("spark.kryoserializer.buffer      " + str(spark_kryoserializer_buffer) + "k\n")


    if configVec[16] == 1:
        spark_rdd_compress = True
    elif configVec[16] == 0:
        spark_rdd_compress = False
    else:
        spark_rdd_compress = "error"
    configList.append("spark.rdd.compress      " + str(spark_rdd_compress) + "\n")


    if configVec[17] == 0:
        spark_serializer = "org.apache.spark.serializer.JavaSerializer"
    elif configVec[17] == 1:
        spark_serializer = "org.apache.spark.serializer.KryoSerializer"
    else:
        spark_serializer = "error"
    # spark_serializer = random.choice(["org.apache.spark.serializer.JavaSerializer",
    #                                   "org.apache.spark.serializer.KryoSerializer"])
    configList.append("spark.serializer      " + str(spark_serializer) + "\n")


    #Memory Management
    # spark_memory_fraction = random.uniform(0.5,1.0)
    spark_memory_fraction = configVec[18]
    configList.append("spark.memory.fraction      " + str(spark_memory_fraction) + "\n")

    # spark_memory_storageFraction = random.uniform(0.5,1.0)
    spark_memory_storageFraction = configVec[19]
    configList.append("spark.memory.storageFraction      " +  str(spark_memory_storageFraction) + "\n")


    if configVec[20] == 1:
        spark_memory_offHeap_enabled = True
    elif configVec[20] == 0:
        spark_memory_offHeap_enabled = False
    else:
        spark_memory_offHeap_enabled = "error"
    configList.append("spark.memory.offHeap.enabled      " +  str(spark_memory_offHeap_enabled) + "\n")


    # spark_memory_offHeap_size = random.randint(10,1000)
    spark_memory_offHeap_size = configVec[21]
    configList.append("spark.memory.offHeap.size      " +  str(spark_memory_offHeap_size) + "m\n")

    # spark_broadcast_blockSize = random.randint(2,128)
    spark_broadcast_blockSize = configVec[22]
    configList.append("spark.broadcast.blockSize      " +  str(spark_broadcast_blockSize) + "m\n")

    # spark_executor_cores = random.randint(1,30)
    spark_executor_cores = configVec[23]
    configList.append("spark.executor.cores      " +  str(spark_executor_cores) + "\n")

    # spark_default_parallelism = random.randint(8,50)
    spark_default_parallelism = configVec[24]
    configList.append("spark.default.parallelism      " +  str(spark_default_parallelism) + "\n")

    # spark_storage_memoryMapThreshold = random.randint(50,500)
    spark_storage_memoryMapThreshold = configVec[25]
    configList.append("spark.storage.memoryMapThreshold      " +  str(spark_storage_memoryMapThreshold) + "m\n")

    # spark_network_timeout = random.randint(20,500)
    spark_network_timeout = configVec[26]
    configList.append("spark.network.timeout      " +  str(spark_network_timeout) + "s\n")

    # Scheduling

    # spark_locality_wait = random.randint(1,10)
    spark_locality_wait = configVec[27]
    configList.append("spark.locality.wait      " +  str(spark_locality_wait) + "s\n")

    # spark_scheduler_revive_interval = random.randint(2,50)
    spark_scheduler_revive_interval = configVec[28]
    configList.append("spark.scheduler.revive.interval      " +  str(spark_scheduler_revive_interval) + "s\n")


    if configVec[29] == 1:
        spark_speculation = True
    elif configVec[29] == 0:
        spark_speculation = False
    else:
        spark_speculation = "error"
    configList.append("spark.speculation      " +  str(spark_speculation) + "\n")

    spark_speculation_interval = configVec[30]
    configList.append("spark.speculation.interval      " +  str(spark_speculation_interval) + "ms\n")

    # spark_speculation_multiplier = random.uniform(1.0,5.0)
    spark_speculation_multiplier = configVec[31]
    configList.append("spark.speculation.multiplier      " +  str(spark_speculation_multiplier) + "\n")

    # spark_speculation_quantile = random.uniform(0,1.0)
    spark_speculation_quantile = configVec[32]
    configList.append("spark.speculation.quantile      " +  str(spark_speculation_quantile) + "\n")

    # spark_task_maxFailures = random.randint(1,8)
    spark_task_maxFailures = configVec[33]
    configList.append("spark.task.maxFailures      " +  str(spark_task_maxFailures) + "\n")


    # confFile = open("/usr/local/spark/conf/spark-defaults.conf","w")

    sparkConf = open("/home/zyx/CLionProjects/spark-2.3.0/conf","w")
    for line in configList:
        sparkConf.write(line)

    sparkConf.close()
    return

# configVec = [12, 17, 34282, 17568, 32, 0, 84, 572, 1, 1, 0, 93,
#              115, 1, 36, 66, 1, 1, 0.5234187804533803, 0.8537448317623525,
#              1, 383, 121, 21, 40, 191, 207, 8, 40, 1, 85, 2.836573715221336,
#              0.38111456222013196, 8]
# writeConf(configVec)


# predict execution by HM model (call R)
def preByhm(inList,datasize):

    # inList[0] = execution time;inList = datasize
    # The length of inList is 36 (the input format of the prediction model).
    # The inList has no practical meaning and is only used to satisfy the length.

    # inList = [103,37,11,36285,1865,92,
    #           1,107,876,1,0,1,30,89,1,123,
    #           124,0,1,0.8482439490352061,
    #           0.6382083374084615,0,784,68,
    #           23,33,448,331,8,15,1,89,
    #           3.489364557076994,0.07325905005015731,8,50]
    
    r.source("/home/zyx/open-source/hm_ga/asplos/model/HModle.RData")
    preList = list(inList)
    preList.insert(0,0)
    preList.append(datasize)
    pre_res=r.preFuc(preList)
    # print(pre_res)
    # r.argtest(inList)
    # print("res is :"+str(res[0]))
    return pre_res
# preByhm()

def enCodein(inList):
    enList=[]

    # The num of executors
    spark_executor_instances = (inList[0]-1) /(100-1)
    enList.append(spark_executor_instances)

    # Application Propertie

    spark_driver_cores = (inList[1]-1) /(20-1)
    enList.append(spark_driver_cores)

    spark_driver_memory = (inList[2]-1024) /(36864-1024)
    enList.append(spark_driver_memory)

    spark_executor_memory = (inList[3]-1024) /(36864-1024)
    enList.append(spark_executor_memory)


    # Shuffle Behavior

    spark_reducer_maxSizeInFlight = (inList[4]-2) /(128-2)
    enList.append(spark_reducer_maxSizeInFlight)

    spark_shuffle_compress = (inList[5]-0) /(1-0)
    if spark_shuffle_compress > 0.5:
        spark_shuffle_compress = 1
    else:
        spark_shuffle_compress = 0
    enList.append(spark_shuffle_compress)

    spark_shuffle_file_buffer = (inList[6]-2) /(128-2)
    enList.append(spark_shuffle_file_buffer)

    spark_shuffle_sort_bypassMergeThreshold = (inList[7]-100) /(1000-100)
    enList.append(spark_shuffle_sort_bypassMergeThreshold)

    spark_shuffle_spill_compress = (inList[8]-0) /(1-0)
    if spark_shuffle_spill_compress > 0.5:
        spark_shuffle_spill_compress = 1
    else:
        spark_shuffle_spill_compress = 0
    enList.append(spark_shuffle_spill_compress)


    # Compression and Serialization

    spark_broadcast_compress = (inList[9]-0) /(1-0)
    if spark_broadcast_compress > 0.5:
        spark_broadcast_compress = 1
    else:
        spark_broadcast_compress = 0
    enList.append(spark_broadcast_compress)

    spark_io_compression_codec = (inList[10]-0) /(2-0)
    if spark_io_compression_codec > (2/3):
        spark_io_compression_codec = 2
    elif spark_io_compression_codec > (1/3):
        spark_io_compression_codec = 1
    else:
        spark_io_compression_codec = 0
    enList.append(spark_io_compression_codec)

    spark_io_compression_snappy_blockSize = (inList[11]-2) /(128-2)
    enList.append(spark_io_compression_snappy_blockSize)

    spark_io_compression_lz4_blockSize = (inList[12]-2) /(128-2)
    enList.append(spark_io_compression_lz4_blockSize)

    spark_kryo_referenceTracking = (inList[13]-0) /(1-0)
    if spark_kryo_referenceTracking > 0.5:
        spark_kryo_referenceTracking = 1
    else:
        spark_kryo_referenceTracking = 0
    enList.append(spark_kryo_referenceTracking)

    spark_kryoserializer_buffer_max = (inList[14]-8) /(128-8)
    enList.append(spark_kryoserializer_buffer_max)

    spark_kryoserializer_buffer = (inList[15]-2) /(128-2)
    enList.append(spark_kryoserializer_buffer)

    spark_rdd_compress = (inList[16]-0) /(1-0)
    if spark_rdd_compress > 0.5:
        spark_rdd_compress = 1
    else:
        spark_rdd_compress = 0
    enList.append(spark_rdd_compress)

    spark_serializer = (inList[17]-0) /(1-0)
    if spark_serializer > 0.5:
        spark_serializer = 1
    else:
        spark_serializer = 0
    enList.append(spark_serializer)

    #Memory Management

    spark_memory_fraction = (inList[18]-0.5) /(1.0-0.5)
    enList.append(spark_memory_fraction)

    spark_memory_storageFraction = (inList[19]-0.5) /(1.0-0.5)
    enList.append(spark_memory_storageFraction)

    spark_memory_offHeap_enabled = (inList[20]-0) /(1-0)
    if spark_memory_offHeap_enabled > 0.5:
        spark_memory_offHeap_enabled = 1
    else:
        spark_memory_offHeap_enabled = 0
    enList.append(spark_memory_offHeap_enabled)

    spark_memory_offHeap_size = (inList[21]-10) /(1000-10)
    enList.append(spark_memory_offHeap_size)

    spark_broadcast_blockSize = (inList[22]-2) /(128-2)
    enList.append(spark_broadcast_blockSize)

    spark_executor_cores = (inList[23]-1) /(30-1)
    enList.append(spark_executor_cores)

    spark_default_parallelism = (inList[24]-8) /(50-8)
    enList.append(spark_default_parallelism)

    spark_storage_memoryMapThreshold = (inList[25]-50) /(500-50)
    enList.append(spark_storage_memoryMapThreshold)

    spark_network_timeout = (inList[26]-20) /(500-20)
    enList.append(spark_network_timeout)


    # Scheduling

    spark_locality_wait = (inList[27]-1) /(10-1)
    enList.append(spark_locality_wait)

    spark_scheduler_revive_interval = (inList[28]-2) /(50-2)
    enList.append(spark_scheduler_revive_interval)

    spark_speculation = (inList[29]-0) /(1-0)
    if spark_speculation > 0.5:
        spark_speculation = 1
    else:
        spark_speculation = 0
    enList.append(spark_speculation)

    spark_speculation_interval = (inList[30]-10) /(100-10)
    enList.append(spark_speculation_interval)

    spark_speculation_multiplier = (inList[31]-1.0) /(5.0-1.0)
    enList.append(spark_speculation_multiplier)

    spark_speculation_quantile = (inList[32]-0) /(1.0-0)
    enList.append(spark_speculation_quantile)

    spark_task_maxFailures = (inList[33]-1) /(8-1)
    enList.append(spark_task_maxFailures)

    return enList

def deCodein(inList):
    deList=[]

    # The num of executors

    spark_executor_instances = inList[0] * (100-1) + 1
    deList.append(round(spark_executor_instances))


    # Application Propertie

    spark_driver_cores = inList[1] * (20-1) + 1
    deList.append(round(spark_driver_cores))

    spark_driver_memory = inList[2] * (36864-1024) + 1024
    deList.append(round(spark_driver_memory))

    spark_executor_memory = inList[3] * (36864-1024) + 1024
    deList.append(round(spark_executor_memory))


    # Shuffle Behavior

    spark_reducer_maxSizeInFlight = inList[4] * (128-2) + 2
    deList.append(round(spark_reducer_maxSizeInFlight))

    if inList[5] > 0.5:
        spark_shuffle_compress = 1
    else:
        spark_shuffle_compress = 0
    deList.append(spark_shuffle_compress)

    spark_shuffle_file_buffer = inList[6] * (128-2) + 2
    deList.append(round(spark_shuffle_file_buffer))

    spark_shuffle_sort_bypassMergeThreshold = inList[7] * (1000-100) + 100
    deList.append(round(spark_shuffle_sort_bypassMergeThreshold))

    if inList[8] > 0.5:
        spark_shuffle_spill_compress = 1
    else:
        spark_shuffle_spill_compress = 0
    deList.append(spark_shuffle_spill_compress)


    # Compression and Serialization

    if inList[9] > 0.5:
        spark_broadcast_compress = 1
    else:
        spark_broadcast_compress = 0
    deList.append(spark_broadcast_compress)

    if inList[10] > (2/3):
        spark_io_compression_codec = 2
    elif inList[10] > (1/3):
        spark_io_compression_codec = 1
    else:
        spark_io_compression_codec = 0
    deList.append(spark_io_compression_codec)

    spark_io_compression_snappy_blockSize = inList[11] * (128-2) + 2
    deList.append(round(spark_io_compression_snappy_blockSize))

    spark_io_compression_lz4_blockSize = inList[12] * (128-2) + 2
    deList.append(round(spark_io_compression_lz4_blockSize))

    if inList[13] > 0.5:
        spark_kryo_referenceTracking = 1
    else:
        spark_kryo_referenceTracking = 0
    deList.append(spark_kryo_referenceTracking)

    spark_kryoserializer_buffer_max = inList[14] * (128-8) + 8
    deList.append(round(spark_kryoserializer_buffer_max))

    spark_kryoserializer_buffer = inList[15] * (128-2) + 2
    deList.append(round(spark_kryoserializer_buffer))

    if inList[16] > 0.5:
        spark_rdd_compress = 1
    else:
        spark_rdd_compress = 0
    deList.append(spark_rdd_compress)

    if inList[17] > 0.5:
        spark_serializer = 1
    else:
        spark_serializer = 0
    deList.append(spark_serializer)


    # Memory Management

    spark_memory_fraction = inList[18] * (1.0-0.5) + 0.5
    deList.append(spark_memory_fraction)

    spark_memory_storageFraction = inList[19] * (1.0-0.5) + 0.5
    deList.append(spark_memory_storageFraction)

    if inList[20] > 0.5:
        spark_memory_offHeap_enabled = 1
    else:
        spark_memory_offHeap_enabled = 0
    deList.append(spark_memory_offHeap_enabled)

    spark_memory_offHeap_size = inList[21] * (1000-10) + 10
    deList.append(round(spark_memory_offHeap_size))

    spark_broadcast_blockSize = inList[22] * (128-2) + 2
    deList.append(round(spark_broadcast_blockSize))

    spark_executor_cores = inList[23] * (30-1) + 1
    deList.append(round(spark_executor_cores))

    spark_default_parallelism = inList[24] * (50-8) + 8
    deList.append(round(spark_default_parallelism))

    spark_storage_memoryMapThreshold = inList[25] * (500-50) + 50
    deList.append(round(spark_storage_memoryMapThreshold))

    spark_network_timeout = inList[26] * (500-20) + 20
    deList.append(round(spark_network_timeout))

    # Scheduling

    spark_locality_wait = inList[27] * (10-1) + 1
    deList.append(round(spark_locality_wait))

    spark_scheduler_revive_interval = inList[28] * (50-2) + 2
    deList.append(round(spark_scheduler_revive_interval))

    if inList[29] > 0.5:
        spark_speculation = 1
    else:
        spark_speculation = 0
    deList.append(spark_speculation)

    spark_speculation_interval = inList[30] * (100-10) + 10
    deList.append(round(spark_speculation_interval))

    spark_speculation_multiplier = inList[31] * (5.0-1.0) + 1.0
    deList.append(spark_speculation_multiplier)

    spark_speculation_quantile = inList[32] * (1.0-0) + 0
    deList.append(spark_speculation_quantile)

    spark_task_maxFailures = inList[33] * (8-1) + 1
    deList.append(round(spark_task_maxFailures))


    return deList

# radomly select vectors from S to initial population
def selectPop(popSize):

    initPlist = []   # initial population (list)
    iniPopFile = open("./init.txt","r")
    inLines =iniPopFile.readlines()

    lenIn = len(inLines)
    if popSize > lenIn:
        print("error: popSize > lenIn")
        return initPlist

    random.shuffle(inLines)
    initPstr = random.sample(inLines,popSize)


    for i in range(popSize):
        initPlist.append((initPstr[i].strip("\n")).split(" "))
        del initPlist[i][0]
        del initPlist[i][-1]

        tmp = []
        for str in initPlist[i]:
            tmp.append(float(str))
        initPlist[i] = enCodein(tmp)
        # print(initPlist[i])

    print(len(initPlist))
    # print(len(initPlist[0]))
    # print(type(initPlist))
    # print(type(initPlist))
    # print(type(initPlist[0]))
    # print(type(initPlist[0][0]))


    iniPopFile.close()
    return initPlist

# selectPop(50)


def main():
    #updateVector()
    # preByhm()
    #deCodein()

    return

