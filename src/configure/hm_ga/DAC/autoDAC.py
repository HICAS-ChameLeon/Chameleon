#!/usr/bin/python3

import os, random

def makeConfig():

    # Generate parameters of the configuration file in the range of values
    configList = []     #Storage profile
    configVec  = []     #Storage profile in vector



    # Writer history-server config (Not in the scope of parameter optimization)

    configList.append("spark.eventLog.enabled             true\n")
    configList.append("spark.history.fs.logDirectory      hdfs://{user}:9000/history\n")
    configList.append("spark.eventLog.dir                 hdfs://{user}:9000/history\n")
    configList.append("spark.yarn.historyServer.address   {user}:18080\n")
    configList.append("spark.eventLog.compress            true\n")
    # configList.append("spark.dynamicAllocation.enabled    true\n")
    # configList.append("spark.dynamicAllocation.minExecutors    1\n")
    # configList.append("spark.dynamicAllocation.maxExecutors    192\n")
    # configList.append("spark.shuffle.service.enabled      true\n")

    # The num of executors

    spark_executor_instances = random.randint(1,100)  #(1,60)
    configVec.append(spark_executor_instances)
    configList.append("spark.executor.instances      " + str(spark_executor_instances) + "\n")

    # Application Propertie

    spark_driver_cores = random.randint(1,30)  #(1,60)
    configVec.append(spark_driver_cores)
    configList.append("spark.driver.cores         " + str(spark_driver_cores) + "\n")

    spark_driver_memory = random.randint(1024,36864)
    configVec.append(spark_driver_memory)
    configList.append("spark.driver.memory        " + str(spark_driver_memory) + "m\n")

    spark_executor_memory = random.randint(1024,36864)
    configVec.append(spark_executor_memory)
    configList.append("spark.executor.memory        " + str(spark_executor_memory) + "m\n")

    # Shuffle Behavior
    spark_reducer_maxSizeInFlight = random.randint(2,128)
    configVec.append(spark_reducer_maxSizeInFlight)
    configList.append("spark.reducer.maxSizeInFlight          " + str(spark_reducer_maxSizeInFlight) + "m\n")

    spark_shuffle_compress = random.choice([True, False])
    configVec.append(spark_shuffle_compress)
    configList.append("spark.shuffle.compress        " + str(spark_shuffle_compress) + "\n")

    spark_shuffle_file_buffer = random.randint(2,128)
    configVec.append(spark_shuffle_file_buffer)
    configList.append("spark.shuffle.file.buffer      " +  str(spark_shuffle_file_buffer) + "k\n")

    spark_shuffle_sort_bypassMergeThreshold = random.randint(100,1000)
    configVec.append(spark_shuffle_sort_bypassMergeThreshold)
    configList.append("spark.shuffle.sort.bypassMergeThreshold  " + str(spark_shuffle_sort_bypassMergeThreshold) + "\n")

    spark_shuffle_spill_compress = random.choice([True, False])
    configVec.append(spark_shuffle_spill_compress)
    configList.append("spark.shuffle.spill.compress      " + str(spark_shuffle_spill_compress) + "\n")


    # Compression and Serialization
    spark_broadcast_compress = random.choice([True, False])
    configVec.append(spark_broadcast_compress)
    configList.append("spark.broadcast.compress      " + str(spark_broadcast_compress) + "\n")

    spark_io_compression_codec = random.choice(["snappy","lz4","lzf"])
    configVec.append(spark_io_compression_codec)
    configList.append("spark.io.compression.codec      " + str(spark_io_compression_codec) + "\n")

    spark_io_compression_snappy_blockSize = random.randint(2,128)
    configVec.append(spark_io_compression_snappy_blockSize)
    configList.append("spark.io.compression.snappy.blockSize  " + str(spark_io_compression_snappy_blockSize) + "k\n")

    spark_io_compression_lz4_blockSize = random.randint(2,128)
    configVec.append(spark_io_compression_lz4_blockSize)
    configList.append("spark.io.compression.lz4.blockSize   " + str(spark_io_compression_lz4_blockSize) + "k\n")

    spark_kryo_referenceTracking = random.choice([True,False])
    configVec.append(spark_kryo_referenceTracking)
    configList.append("spark.kryo.referenceTracking      " + str(spark_kryo_referenceTracking) + "\n")

    spark_kryoserializer_buffer_max = random.randint(8,128)
    configVec.append(spark_kryoserializer_buffer_max)
    configList.append("spark.kryoserializer.buffer.max      " + str(spark_kryoserializer_buffer_max) + "m\n")

    spark_kryoserializer_buffer = random.randint(2,128)
    configVec.append(spark_kryoserializer_buffer)
    configList.append("spark.kryoserializer.buffer      " + str(spark_kryoserializer_buffer) + "k\n")

    spark_rdd_compress = random.choice(["True","False"])
    configVec.append(spark_rdd_compress)
    configList.append("spark.rdd.compress      " + str(spark_rdd_compress) + "\n")

    # spark_serializer = random.choice(["org.apache.spark.serializer.JavaSerializer",
    #                                   "org.apache.spark.serializer.KryoSerializer"])

    spark_serializer = random.choice(["0",
                                      "1"])
    configVec.append(spark_serializer)
    configList.append("spark.serializer      " + str(spark_serializer) + "\n")


    #Memory Management
    spark_memory_fraction = random.uniform(0.5,1.0)
    configVec.append(spark_memory_fraction)
    configList.append("spark.memory.fraction      " + str(spark_memory_fraction) + "\n")

    spark_memory_storageFraction = random.uniform(0.5,1.0)
    configVec.append(spark_memory_storageFraction)
    configList.append("spark.memory.storageFraction      " +  str(spark_memory_storageFraction) + "\n")

    spark_memory_offHeap_enabled = random.choice([True,False])
    configVec.append(spark_memory_offHeap_enabled)
    configList.append("spark.memory.offHeap.enabled      " +  str(spark_memory_offHeap_enabled) + "\n")

    spark_memory_offHeap_size = random.randint(10,1000)
    configVec.append(spark_memory_offHeap_size)
    configList.append("spark.memory.offHeap.size      " +  str(spark_memory_offHeap_size) + "m\n")

    spark_broadcast_blockSize = random.randint(2,128)
    configVec.append(spark_broadcast_blockSize)
    configList.append("spark.broadcast.blockSize      " +  str(spark_broadcast_blockSize) + "m\n")

    spark_executor_cores = random.randint(1,30)
    configVec.append(spark_executor_cores)
    configList.append("spark.executor.cores      " +  str(spark_executor_cores) + "\n")

    spark_default_parallelism = random.randint(8,50)
    configVec.append(spark_default_parallelism)
    configList.append("spark.default.parallelism      " +  str(spark_default_parallelism) + "\n")

    spark_storage_memoryMapThreshold = random.randint(50,500)
    configVec.append(spark_storage_memoryMapThreshold)
    configList.append("spark.storage.memoryMapThreshold      " +  str(spark_storage_memoryMapThreshold) + "m\n")

    spark_network_timeout = random.randint(20,500)
    configVec.append(spark_network_timeout)
    configList.append("spark.network.timeout      " +  str(spark_network_timeout) + "s\n")


    # Scheduling

    spark_locality_wait = random.randint(1,10)
    configVec.append(spark_locality_wait)
    configList.append("spark.locality.wait      " +  str(spark_locality_wait) + "s\n")

    spark_scheduler_revive_interval = random.randint(2,50)
    configVec.append(spark_scheduler_revive_interval)
    configList.append("spark.scheduler.revive.interval      " +  str(spark_scheduler_revive_interval) + "s\n")

    spark_speculation = random.choice([True,False])
    configVec.append(spark_speculation)
    configList.append("spark.speculation      " +  str(spark_speculation) + "\n")

    spark_speculation_interval = random.randint(10,100)
    configVec.append(spark_speculation_interval)
    configList.append("spark.speculation.interval      " +  str(spark_speculation_interval) + "ms\n")

    spark_speculation_multiplier = random.uniform(1.0,5.0)
    configVec.append(spark_speculation_multiplier)
    configList.append("spark.speculation.multiplier      " +  str(spark_speculation_multiplier) + "\n")

    spark_speculation_quantile = random.uniform(0,1.0)
    configVec.append(spark_speculation_quantile)
    configList.append("spark.speculation.quantile      " +  str(spark_speculation_quantile) + "\n")

    spark_task_maxFailures = random.randint(1,8)
    configVec.append(spark_task_maxFailures)
    configList.append("spark.task.maxFailures      " +  str(spark_task_maxFailures) + "\n")




    #confFile = open("/usr/local/spark/conf/spark-defaults.conf","w")

    confFile = open("spark-defaults.conf", "w")
    for line in configList:
        confFile.write(line)



    confFile.close()


    return configVec

def preData():

    # prepare data
    print("Need to prepare data? please input yes or no ! ")
    inStr = input()
    if inStr == "yes":
        predata = True
    elif inStr == "no":
        predata = False
    else:
        print("Input error.....")
        return False

    # Determine whether you need to prepare data based on the input.
    if predata == True :
        print("prepare data")
        command1 = "../prepare/prepare.sh"
        result = os.system(command1)
        if result != 0:
            print("Error preparing data........")
            return False

    print("prepare data finished...")
    return True

def runJob():


    print("\n Job is running...")
    result = 0
     # /home/zyx/app/Hibench-7.0/bin/workloads/micro/wordcount/spark/run.sh
    # run job
    command = "{run.sh}"
    result = os.system(command)

    return result



def makeResult(total):
    # /home/zyx/app/Hibench-7.0/report/hibench.report
    # read execution time from hibench.report
    benFile = open("{report}/hibench.report","r")
    benFile.readline()  # skip the first line

    timeVec =[]   # store execution time
    dsizeVec = []  # stort input datasize


    for i in range(total):
        line = benFile.readline()
        line = ' '.join((line.strip("\n")).split())    #replace "," with consecutive spaces
        word = list(line.split(" "))
        timeVec.append(word[-3])
        dsizeVec.append(word[-4])


    # read config from confVec.txt (vector)
    # /home/zyx/app/Hibench-7.0/report/confVec.txt
    confVec = []
    confVecFile = open("{report}/confVec.txt","r")
    for i in range(total):
        line = confVecFile.readline()
        line = line.strip("\n")
        line = line.strip(" ")
        confVec.append(line.split(" "))



    # combine config vector and execution time  and save comVec in file(time_conf.txt)
    comVecFile = open("{report}/time_conf.txt","w")
    comVec = []
    for i in range(total):
        tmpVec = []
        tailVec = []
        tmpVec.append(timeVec[i])
        tailVec.append(dsizeVec[i])

        # tmpVec = tmpVec + confVec[i]
        tmpVec = tmpVec + confVec[i] + tailVec
        # print(len(tmpVec))

        pstr = ""
        for tmp in range(len(tmpVec)):
            # pstr = pstr + str(tmpVec[tmp]) + ","
            if tmp == (len(tmpVec) - 1):
                pstr = pstr + str(tmpVec[tmp])
            else:
                pstr = pstr + str(tmpVec[tmp]) + " "

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


# noinspection PyInterpreter
def main():
    # prepare data
   # result = preData()
   # if result == False:
   #     return

    #run job
    total = 300    # the total num of rows
    numOfail = 0  # record the number of failed tasks
    numOfSuc = 0  # record the number of succeed tasks

    # remove the old hibench.report and touch a new hibench.report
    cmdrm = "rm -rf {report}/hibench.report"
    cmdtouch = "touch {report}/hibench.report"
    os.system(cmdrm)
    os.system(cmdtouch)

    firstline = "Type         Date       Time     Input_data_size      Duration(s)"+ \
                "          Throughput(bytes/s)  Throughput/node\n"
    firFile = open("{report}/hibench.report","w")
    firFile.write(firstline)
    firFile.close()

    # clear error log folder:errorDAC
    cmdrmlog = "rm -rf {report}/errorDAC/*"
    os.system(cmdrmlog)

    confVecFile = open("./confVec.txt", "w")
    confVecFile.close()

    errorVecFile = open("./err_timeConfVec.txt", "w")
    errorVecFile.close()


    # define the excution time of failed job (positive infinity)
    PosInf = 50000   # second

    for i in range(total):
        print("The num of finished jobs:" + str(i))
        print("Succeed: "+ str(numOfSuc))
        print("Failed: "+ str(numOfail))

        configVec = makeConfig()   # make config
        res = runJob()
        if res == 0:
            numOfSuc += 1
            tmpStr = ""
            for k in configVec:
                tmpStr = tmpStr + str(k) + " "  # note: there is an space at the end of str

            tmpStr = tmpStr + "\n"

            confVecFile = open("{report}/confVec.txt", "a")
            # confVecFile = open("./confVec.txt", "w")
            confVecFile.write(tmpStr)
            confVecFile.close()

        else:
            numOfail += 1

            # save error log
            cmdlog = "cp {report}/terasort/spark/bench.log" + \
                     " {report}/errorDAC/" + str(i) + "error.log"
            os.system(cmdlog)

            tmpStr = ""
            for k in configVec:
                tmpStr = tmpStr + str(k) + " "  # note: there is an space at the end of str

            tmpStr = tmpStr + "\n"

            # save the excution time and confVec of failed job
            errorVecFile = open("{report}/err_timeConfVec.txt", "a")
            errorVecFile.write(str(PosInf)+" "+tmpStr)
            errorVecFile.close()



    # Read run time Generate configuration-time data
    makeResult(numOfSuc)    #  Note: use numOfsuc ,not total,because some job may be failed


    # print result
    print("total = " + str(total) + "  finished ...")
    print("succeed: " + str(numOfSuc))
    print("failed: " + str(numOfail))



# makeResult(20)
main()
