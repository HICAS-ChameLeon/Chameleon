# coding: utf-8
try:  # python3
    from io import BytesIO
    import urllib.request
except ImportError:  # python2
    import StringIO
    import urllib
import random, csv, util, time, os


'''
   makeRandomConfig：generate data by random function
   :returns configuration parameters' name, and the configuration parameter values
'''
def makeRandomConfig():
    record = []
    job_heap = random.randrange(1024,2048)
    record.append(job_heap)
    task_heap = random.randrange(2048, 4097)
    record.append(task_heap)
    slots = random.randint(1, 2)
    print(slots)
    record.append(slots)
    memory_off_heap = random.sample(['true', 'false'], 1)[0]
    if memory_off_heap == 'true':
        record.append(1)
    else:
        record.append(0)
    # memory_size=int(data[16].split()[-1])
    memory_fraction = random.sample([0.4, 0.5, 0.6, 0.7, 0.8], 1)[0]
    record.append(memory_fraction)
    '''加个K'''
    segment_size = random.sample([32, 64, 128, 256, 512, 1024], 1)[0]
    record.append(segment_size)
    # network_memory_fraction = random.sample([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9], 1)[0]
    network_memory_fraction = random.randint(1, 5) / 10.0
    record.append(network_memory_fraction)
    network_memory_min = pow(2, random.randint(5, 10))
    record.append(network_memory_min)
    network_memory_max = random.randint(1024, 4097)
    record.append(network_memory_max)
    '''=================================================='''
    net_num_arenas = slots
    net_client_thread = slots
    record.append(net_num_arenas)
    net_server_thread = random.randint(1, 4)
    record.append(net_server_thread)
    record.append(net_client_thread)
    net_sendReceiveBufferSize = random.randint(93087, 186174)
    record.append(net_sendReceiveBufferSize)

    blob_fetch_reties = random.randrange(50, 101)
    record.append(blob_fetch_reties)
    blob_numcurrent = random.randrange(50, 101)
    record.append(blob_numcurrent)
    blob_backlog = random.randrange(900, 2001)
    record.append(blob_backlog)
    jobmanager_tdd_offload_minsize = random.randint(900, 4096)
    record.append(jobmanager_tdd_offload_minsize)

    '''++++++++++++++++++++++++++++++++++++++++++++++++++'''
    akka_framesize = random.randrange(6, 21, 1)
    record.append(akka_framesize)
    akka_watch_thread = random.randint(8, 21)
    record.append(akka_watch_thread)

    fs_overwrite_files = random.sample(['true', 'false'], 1)[0]
    if fs_overwrite_files == 'true':
        record.append(1)
    else:
        record.append(0)
    fs_output_cdirectory = random.sample(['true', 'false'], 1)[0]
    if fs_output_cdirectory == 'true':
        record.append(1)
    else:
        record.append(0)

    compiler_maxline_samples = random.randint(9, 25)
    record.append(compiler_maxline_samples)
    compiler_minline_samples = random.randint(2, compiler_maxline_samples // 2 + 1)
    record.append(compiler_minline_samples)
    ###MBytes
    compiler_max_sampleslen = random.randint(1, 10)
    record.append(compiler_max_sampleslen)

    Runtime_hash = random.sample(['true', 'false'], 1)[0]
    if Runtime_hash == 'true':
        record.append(1)
    else:
        record.append(0)
    runtime_maxfan = random.randint(120, 170)
    record.append(runtime_maxfan)
    runtime_threadhold = random.sample([0.5, 0.6, 0.7, 0.8, 0.9], 1)[0]
    record.append(runtime_threadhold)

    totalSlots = slots*1
    record.append(totalSlots)
    return record

'''
   write2Configure： write configurations to the config file
   :parameter conf：the path to the config file
'''
def write2Configure(conf):
    with open('./flink-conf.yaml', 'r+') as file_to_read:
        data = file_to_read.readlines()
        file_to_read.seek(0)
        file_to_read.truncate()
        #job_heap
        data[23]=data[23].split()[0]+' '+str(int(round(conf[0]))) + '\n'
        # task_heap
        data[25]=data[25].split()[0]+' '+str(int(round(conf[1]))) + '\n'
        #slot
        data[27] = data[27].split()[0] + ' ' + str(int(round(conf[2]))) + '\n'
        # memory_off_heap
        memory_off_heap = "false"
        if conf[3] == 1:
            memory_off_heap = "true"
        data[31] = data[31].split()[0] + ' ' + memory_off_heap + '\n'
        #memory_fraction
        data[33]=data[33].split()[0]+' '+str(round(conf[4],1)) + '\n'

        #segment_size
        data[36]=data[36].split()[0]+' '+str(int(round(conf[5]))) + ' K' + '\n'

        #network_memory_fraction
        data[42]=data[42].split()[0]+' '+str(round(conf[6],1)) + '\n'
        #network_memory_min
        data[44] = data[44].split()[0] + ' ' + str(int(round(conf[7]))) + ' MB' + '\n'
        #network_memory_max
        data[46] = data[46].split()[0] + ' ' + str(int(round(conf[8]))) + ' MB' + '\n'

        #net_num_arenas
        data[48]=data[48].split()[0]+' '+str(int(round(conf[9]))) + '\n'
        #net_client_thread
        data[50]=data[50].split()[0]+' '+str(int(round(conf[10]))) + '\n'
        # net_server_thread
        data[52]=data[52].split()[0]+' '+str(int(round(conf[11]))) + '\n'
        #net_sendReceiveBufferSize
        data[54]=data[54].split()[0]+' '+str(int(round(conf[12]))) + '\n'


        #blob_fetch_reties
        data[56]=data[56].split()[0]+' '+str(int(round(conf[13]))) + '\n'
        #blob_numcurrent
        data[58]=data[58].split()[0]+' '+str(int(round(conf[14]))) + '\n'
        #blob_backlog
        data[60] = data[60].split()[0] + ' ' + str(int(round(conf[15]))) + '\n'
        '''++++++++++++++++++++++++++++++++++++++++++++++++++'''

        #jobmanager_tdd_offload_minsize
        data[64] = data[64].split()[0] + ' ' + str(int(round(conf[16]))) + '\n'

        # akka_framesize
        data[68]=data[68].split()[0]+' '+str(int(round(conf[17]))) + ' MB' + '\n'
        # akka_watch_thread
        data[70] = data[70].split()[0] + ' ' + str(int(round(conf[18]))) + '\n'

        #fs_overwrite_files
        fs_overwrite_files = "false"
        if conf[19] == 1:
            fs_overwrite_files = "true"
        data[75] = data[75].split()[0] + ' ' + fs_overwrite_files + '\n'
        #fs_output_cdirectory
        fs_output_cdirectory = "false"
        if conf[20] == 1:
            fs_output_cdirectory = "true"
        data[78] = data[78].split()[0] + ' ' + fs_output_cdirectory + '\n'

        #compiler_maxline_samples
        data[82] = data[82].split()[0] + ' ' + str(int(round(conf[21]))) + '\n'
        #compiler_minline_samples
        data[84] = data[84].split()[0] + ' ' + str(int(round(conf[22]))) + '\n'
        #compiler_max_sampleslen
        data[86] = data[86].split()[0] + ' ' + str(int(round(conf[23]))) + ' MB' + '\n'

        # runtime_hash
        runtime_hash = "false"
        if conf[24] == 1:
            runtime_hash = "true"
        data[93] = data[93].split()[0] + ' ' + runtime_hash + '\n'
        #runtime_maxfan
        data[95] = data[95].split()[0] + ' ' + str(int(round(conf[25]))) + '\n'
        # runtime_threadhold
        data[97]=data[97].split()[0]+' '+str(round(conf[26],1)) + '\n'

        # default parallelism
        data[29] = data[29].split()[0] + ' ' + str(int(round(conf[27]))) + '\n'

        file_to_read.writelines(data)
        file_to_read.close()

def collectMetrics(dataDir, conf, perf):
    # control input data speed
    # get performance metrics, perf is a list
    record = []
    record.extend(conf)
    record.append(perf)
    f1 = open(dataDir, 'a+')
    cf1 = csv.writer(f1, delimiter=',', lineterminator='\n')
    cf1.writerow(record)
    f1.close()

def dealOutlier(dataDir, conf):
    f1 = open(dataDir, 'a+')
    cf1 = csv.writer(f1, delimiter=',', lineterminator='\n')
    conf.append(1000000)
    cf1.writerow(conf)
    f1.close()


def collectData(dataDir, slaves, confValue):
    # os.system("rm /FLINK_HOME/log/*")
    write2Configure(confValue)
    re = ''
    #### cancel job
    #util.cancelJob()
    
    #### operators' parallelism
    #slots1 = random.randint(confValue[-1] / 2, confValue[-1] - 1)
    #slots2 = confValue[-1]-slots1
    # os.system("ssh root@10.186.133.175 '/export/App/bin/flink -d run /export/App/bin/min.jar " + str(slots1) + " " + str(slots2) + "'")
    
    os.system( 'nohup /home/zyx/open-source/flink-1.4.2/build-target/bin/mesos-appmaster.sh &')
    for i in slaves:
        print(i)
        #os.system('ssh zyx@' +str(i) +" '/home/zyx/open-source/flink-1.4.2/build-target/bin/stop-cluster.sh'")
        # os.system('ssh -t ' + str(i) + ' "rm /FLINK_HOME/log/*"')
        os.system('scp ./flink-conf.yaml zyx@' + str(i) + ':/home/zyx/open-source/flink-1.4.2/build-target/conf')
        #### start the TaskManager
       # os.system('ssh zyx@' +str(i) +" '/home/zyx/open-source/flink-1.4.2/build-target/bin/start-cluster.sh'")
      #  
    
    time.sleep(28)
    ### test  whether the cluster is successfully started
    p = os.popen('sh ./isTask.sh ' + str(slaves[-1]))
    re = p.read()
    p.close()
    print("fffff")
    #### if cluster is successfully started
    if re != '\n':
        print("dddd")
        if re.strip('\n').strip('\r').split()[-1] == 'MesosApplicationMasterRunner':
            print("dddd")
            try:
                ### submitting the job
                # flink run ../flink-1.4.2/build-target/examples/streaming/TopSpeedWindowing.jar 
                os.system("ssh zyx@172.20.110.100 '/home/zyx/open-source/flink-1.4.2/build-target/bin/flink run /home/zyx/open-source/flink-1.4.2/build-target/examples/streaming/TopSpeedWindowing.jar'")
               # os.system("ssh zyx@172.20.110.100 '/home/zyx/open-source/flink/build-target/bin/flink run /home/zyx/open-source/flink/build-target/examples/batch/EnumTriangles.jar'")

                #jobid = util.getJobStatus()
                jobid = util.getFlinkFinishJobid()
                print(jobid)
                if jobid == '':
                    dealOutlier(dataDir,confValue)
                else:
                    #### profile data in 10 minutes
                    #time.sleep(150)
                    #### collect the metrics
                    perf = util.getPerformanceMetrics(jobid)
                    collectMetrics(dataDir, confValue, perf)
            except Exception: #job failed
                print("config fail")
                dealOutlier(dataDir,confValue)
    else: #### TM failed to start
        print("TM fail to start")
        dealOutlier(dataDir, confValue)
