
#!/usr/bin/python3

import os, random



def writeConf(configVec):

    configList = []     #Storage profile

    # Writer history-server config (Not in the scope of parameter optimization)
    configList.append("spark.eventLog.enabled             true\n")
    configList.append("spark.history.fs.logDirectory      hdfs://huawei-master:9000/history\n")
    configList.append("spark.eventLog.dir                 hdfs://huawei-master:9000/history\n")
    configList.append("spark.yarn.historyServer.address   huawei-master:18080\n")
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

    sparkConf = open("/usr/local/spark/conf/spark-defaults.conf","w")
    for line in configList:
        sparkConf.write(line)

    sparkConf.close()
    return


def main():
    conf_min = [
        [49, 16, 21048, 27706, 106, 1, 40, 541, 0, 1, 1, 124, 37, 0, 106, 99,
         0, 0, 0.6126621527017662, 0.6972037022678224, 1, 278, 97, 19, 22,
         210, 442, 8, 32, 1, 13, 1.8754775972239157, 0.8330531379860201, 6],
        [67, 8, 34226, 27937, 102, 1, 86, 513, 0, 1, 2, 125,
         80, 1, 77, 84, 1, 0, 0.5617815608899226, 0.5004708169589799, 0, 500,
         61, 20, 47, 349, 438, 8, 15, 1, 98, 4.7404919397693135,
         0.9367475798073412, 4],
        [61, 17, 24514, 28175, 124, 1, 7, 763, 0, 0, 1, 73, 103,
         0, 126, 102, 1, 1, 0.7924670347961189, 0.7187560313231751, 1, 179, 96, 9, 45,
         425, 408, 6, 46, 0, 28, 2.801270937274424, 0.1327945597258745, 5]]

    conf_max = [
        [2, 7, 24535, 10067, 25, 0, 55, 400, 1, 0, 1, 20, 60, 0, 92, 79, 0, 0,
         0.8682061423425897, 0.8134025552990605, 1, 853, 33, 22, 22, 459, 159,
         9, 13, 1, 97, 1.495069591955727, 0.5661456560476198, 5],
        [4, 16, 34376, 9185, 46, 0, 43, 688, 1, 0, 0, 104, 76, 0, 75, 91, 0, 0,
         0.9023539891844751,0.9340488056544626, 0, 965, 20, 11, 39, 267, 321, 5,
         28, 1, 86, 4.816730073379041, 0.853094084174385, 1]
    ]

    conf_min_max = conf_min + conf_max
    i = 0
    suc = 0
    fail = 0
    failtask = []
    for configVec in conf_min_max:
        writeConf(configVec)
        cmd = "./run.sh"
        result = os.system(cmd)
        if result == 0:
            suc += 1
        else:
            fail += 1
            failtask.append(i)
        i +=1

    print("suc is ",suc)
    print("fail is ",fail,failtask)

    return



