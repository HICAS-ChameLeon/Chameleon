### coding: utf-8
from __future__ import division
from __future__ import print_function

try:  # python3
    from io import BytesIO
    import urllib.request
except ImportError:  # python2
    import StringIO
    import urllib

import pycurl
import json, os, time, random,multiprocessing,csv


def handle_html(url):
    try:
        b = BytesIO()  # python2 uses StringIO()
    except Exception:
        b = StringIO()
    c = pycurl.Curl()
    c.setopt(pycurl.URL, url)
    c.setopt(pycurl.HTTPHEADER, ["Accept: application/json"])
    c.setopt(pycurl.WRITEFUNCTION, b.write)
    c.perform()

    html = b.getvalue().decode('UTF-8')
    hjson = json.loads(html)

    c.close()
    b.close()
    return hjson

def configure(path, string, raw, value):
    with open(path, 'r+') as file_to_read:
        lines = file_to_read.readlines()
        file_to_read.seek(0)
        file_to_read.truncate()
        lines[raw - 1] = string + '          ' + str(value) + '\n'
        file_to_read.writelines(lines)
        file_to_read.close()

def makeRandomConfig():
    record = []
    job_heap = random.randint(1024,4097)
    record.append(job_heap)
    task_heap = random.randint(job_heap*2, 10240102401024010240102401024010240102401024010240)
    record.append(task_heap)
    slots = random.randrange(3, 25,3)
    record.append(slots)
    memory_off_heap = random.sample(['true', 'false'], 1)[0]
    if memory_off_heap == 'true':
        record.append(1)
    else:
        record.append(0)
    # memory_size=int(data[16].split()[-1])
    memory_fraction = random.sample([0.4, 0.5, 0.6, 0.7, 0.8], 1)[0]
    record.append(memory_fraction)
    memory_preallocate = random.sample(['true', 'false'], 1)[0]
    if memory_preallocate == 'true':
        record.append(1)
    else:
        record.append(0)
    '''加个K'''
    segment_size = random.sample(['32', '64', '128', '256', '512', '1024', '2048', '4096'], 1)[0]
    record.append(int(segment_size))
    # network_memory_fraction = random.sample([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9], 1)[0]
    network_memory_fraction=random.randint(1,10)/10.0
    record.append(network_memory_fraction)
    network_memory_min=pow(2,random.randint(5,8))
    record.append(network_memory_min)
    network_memory_max=random.randint(1024,2049)
    record.append(network_memory_max)
    '''=================================================='''
    net_num_arenas = random.randint(1, 8)
    record.append(net_num_arenas)
    net_server_thread = random.randint(1, 8)
    record.append(net_server_thread)
    net_client_thread = random.randint(1, 8)
    record.append(net_client_thread)
    net_sendReceiveBufferSize=random.randint(763659,1527318)
    record.append(net_sendReceiveBufferSize)

    blob_fetch_reties = random.randrange(50, 101, 10)
    record.append(blob_fetch_reties)
    blob_numcurrent = random.randrange(50, 101, 10)
    record.append(blob_numcurrent)
    blob_backlog=random.randrange(900,2000,100)
    record.append(blob_backlog)
    jobmanager_tdd_offload_minsize=random.randint(900,4097)
    record.append(jobmanager_tdd_offload_minsize)

    '''++++++++++++++++++++++++++++++++++++++++++++++++++'''
    akka_framesize = random.randrange(6, 21, 1)
    record.append(akka_framesize)
    akka_watch_thread = random.randint(8, 22)
    record.append(akka_watch_thread)

    fs_overwrite_files=random.sample(['true', 'false'], 1)[0]
    if fs_overwrite_files == 'true':
        record.append(1)
    else:
        record.append(0)
    fs_output_cdirectory=random.sample(['true', 'false'], 1)[0]
    if fs_output_cdirectory == 'true':
        record.append(1)
    else:
        record.append(0)

    compiler_maxline_samples=random.randint(9,21)
    record.append(compiler_maxline_samples)
    compiler_minline_samples=random.randint(2,compiler_maxline_samples//2+1)
    record.append(compiler_minline_samples)
    ###MiBytes
    compiler_max_sampleslen=random.randint(1,10)
    record.append(compiler_max_sampleslen)


    Runtime_hash = random.sample(['true', 'false'], 1)[0]
    if Runtime_hash == 'true':
        record.append(1)
    else:
        record.append(0)
    runtime_maxfan=random.randint(120,200)
    record.append(runtime_maxfan)
    runtime_threadhold = random.sample([0.5, 0.6, 0.7, 0.8, 0.9], 1)[0]
    record.append(runtime_threadhold)

    res = [str(job_heap), str(task_heap), str(slots), memory_off_heap, str(memory_fraction),memory_preallocate,
           str(segment_size)+' KiB', str(network_memory_fraction), str(network_memory_min)+' MiB',str(network_memory_max)+' MiB',
           str(net_num_arenas), str(net_client_thread), str(net_server_thread), str(net_sendReceiveBufferSize),
           str(blob_fetch_reties), str(blob_numcurrent), str(blob_backlog), str(jobmanager_tdd_offload_minsize),
           str(akka_framesize)+' MiB', str(akka_watch_thread), fs_overwrite_files,fs_output_cdirectory,str(compiler_maxline_samples),
           str(compiler_minline_samples), str(compiler_max_sampleslen)+' MiB', Runtime_hash, str(runtime_maxfan), str(runtime_threadhold)
           ]

    return res,record

# makeRandomConfig()

# 获取配置文件中第l行的string字段的值
#    :parameter
#        path: 配置文件所在的路径
#        string:要查找的配置参数项
#        raw: 配置参数项在该文件的行数
def getConfigure(path, string, raw):
    with open(path, 'r') as file_to_read:
        lines = file_to_read.readlines()
        file_to_read.seek(0)
        # file_to_read.truncate()
        value = lines[raw - 1].split()[-1]
        file_to_read.close()
    return value

def getKafkatopic(pro):
    produceNum = getConfigure('./conf/hibench.conf', 'hibench.streambench.datagen.producerNumber', 111)
    recordPerInterval = getConfigure('./conf/hibench.conf', 'hibench.streambench.datagen.recordsPerInterval', 107)
    intervalSpan = getConfigure('./conf/hibench.conf', 'hibench.streambench.datagen.intervalSpan', 105)
    recordsPreSecond = int(recordPerInterval) * 1000 * int(produceNum) / int(intervalSpan)
    # t = os.popen('./bin/workloads/streaming/'+pro+'common/metrics_reader.sh')
    string = 'FLINK_' + pro + '_' + produceNum + '_' + recordPerInterval + '_' + intervalSpan
    topic = os.popen("sh flink_benchmark.sh " + string)
    # print(topic.read().split('\n')[0])
    reportTopic = topic.read().split('\n')[0]
    topic.close()
    return reportTopic, recordsPreSecond

def readCVS(filename):
    with open(filename) as f:
        reader = csv.DictReader(f)
        # count=0
        throughput=0.0
        latency=0.0
        mean=0.0
        # perf=0.0
        for row in reader:
            # Max TemperatureF是表第一行的某个数据，作为key
            # count = row['count']
            throughput=float(row['throughput(msgs/s)'])
            mean=float(row['mean_latency(ms)'])
            latency=float(row['p99_latency(ms)'])
        f.close()
        # if(throughput==0.0 or latency==0.0):
        #     return None
        # else:
        #     # perf = float(throughput * 1000 / latency)
        res=[throughput,latency,mean]
        return res

def write2Configure(conf):
    # str(job_heap), str(task_heap), str(slots), memory_off_heap, str(memory_fraction), memory_preallocate,
    # str(segment_size) + ' KiB', str(network_memory_fraction), str(network_memory_min) + ' MiB', str(
    #     network_memory_max) + ' MiB',
    # str(net_num_arenas), str(net_client_thread), str(net_server_thread), str(net_sendReceiveBufferSize),
    # str(blob_fetch_reties), str(blob_numcurrent), str(blob_backlog), str(jobmanager_tdd_offload_minsize),
    # str(akka_framesize) + ' MiB', str(akka_watch_thread), fs_overwrite_files, fs_output_cdirectory, str(
    #     compiler_maxline_samples),
    # str(compiler_minline_samples), str(compiler_max_sampleslen), Runtime_hash, str(runtime_threadhold),
    with open('/home/gy/flink-1.4.2/conf/flink-conf.yaml', 'r+') as file_to_read:
        data = file_to_read.readlines()
        file_to_read.seek(0)
        file_to_read.truncate()
        # data[raw - 1] = string + '          ' + str(value) + '\n'
        #job_heap
        data[6]=data[6].split()[0]+' '+conf[0]+ '\n'
        # task_heap
        data[8]=data[8].split()[0]+' '+conf[1]+ '\n'
        #slot
        configure('/home/gy/HiBench/conf/flink.conf','hibench.streambench.flink.parallelism',6,conf[2])
        # memory_off_heap
        data[14]=data[14].split()[0]+' '+conf[3]+ '\n'
        #memory_fraction
        data[18]=data[18].split()[0]+' '+conf[4]+ '\n'

        #memory_preallocate
        data[20]=data[20].split()[0]+' '+conf[5]+ '\n'

        #segment_size
        data[22]=data[22].split()[0]+' '+conf[6]+ '\n'

        #network_memory_fraction
        data[30]=data[30].split()[0]+' '+conf[7]+ '\n'
        #network_memory_min
        data[32] = data[32].split()[0] + ' ' + conf[8] + '\n'
        #network_memory_max
        data[34] = data[34].split()[0] + ' ' + conf[9] + '\n'

        #net_num_arenas
        data[36]=data[36].split()[0]+' '+conf[10]+ '\n'
        #net_client_thread
        data[38]=data[38].split()[0]+' '+conf[11]+ '\n'
        # net_server_thread
        data[40]=data[40].split()[0]+' '+conf[12]+ '\n'
        #net_sendReceiveBufferSize
        data[42]=data[42].split()[0]+' '+conf[13]+ '\n'


        #blob_fetch_reties
        data[44]=data[44].split()[0]+' '+conf[14]+ '\n'
        #blob_numcurrent
        data[46]=data[46].split()[0]+' '+conf[15]+ '\n'
        #blob_backlog
        data[48] = data[48].split()[0] + ' ' + conf[16] + '\n'
        '''++++++++++++++++++++++++++++++++++++++++++++++++++'''

        #jobmanager_tdd_offload_minsize
        data[54] = data[54].split()[0] + ' ' + conf[17] + '\n'

        # akka_framesize
        data[58]=data[58].split()[0]+' '+conf[18] +  '\n'
        # akka_watch_thread
        data[60] = data[60].split()[0] + ' ' + conf[19] + '\n'

        #fs_overwrite_files
        data[64] = data[64].split()[0] + ' ' + conf[20] + '\n'
        #fs_output_cdirectory
        data[66] = data[66].split()[0] + ' ' + conf[21] + '\n'

        #compiler_maxline_samples
        data[70] = data[70].split()[0] + ' ' + conf[22] + '\n'
        #compiler_minline_samples
        data[72] = data[72].split()[0] + ' ' + conf[23] + '\n'
        #compiler_max_sampleslen
        data[74] = data[74].split()[0] + ' ' + conf[24] + '\n'

        # runtime_hash
        data[101]=data[101].split()[0]+' '+conf[25]+ '\n'
        #runtime_maxfan
        data[103] = data[103].split()[0] + ' ' + conf[26] + '\n'
        # runtime_threadhold
        data[105]=data[105].split()[0]+' '+conf[27]+ '\n'

        file_to_read.writelines(data)
        file_to_read.close()

def put2kafka(pro):
    os.system('./bin/workloads/streaming/' + pro + '/prepare/dataGen.sh')
    # time.sleep(600)


def runFlink(pro):
    os.system('./bin/workloads/streaming/' + pro + '/flink/run.sh')


def multipro(pro):
    p = multiprocessing.Process(target=runFlink, args=(pro,))
    p.daemon = True
    p.start()
    put2kafka(pro)


def change_data(data):
    # 进行json数据转换
    try:
        # separators 会把对应符号前后的空格去掉，网络传输中，空格没有意义
        # 还可以通过sort_keys进行按字典可以排序，字典才有效，网络传输一般都用json数据格式
        return json.dumps(data, separators=[',', ':'], sort_keys=True)
    except Exception as e:
        print(e)
        return None

def dealData(record,pro):
    # t=2500
    #for t in (30000,25000,20000,12500,5000,2500):
  #  for t in (6250,12500,37500,50000,62500,75000,125000):
     for t in (6250,12500,37500,50000,62500,75000,125000):
        row=[]
        row.extend(record)
#        configure('./conf/hibench.conf', 'hibench.streambench.datagen.recordsPerInterval', 107, t)
        # os.system('./bin/workloads/streaming/'+pro+'/prepare/genSeedDataset.sh')
        multipro(pro)
        topic, recordsPreSecond = getKafkatopic(pro)
        mbPreSecond = t*2500/100
        os.system('./bin/workloads/streaming/' + pro + '/common/metrics_reader.sh ' + topic)
        jID = os.popen('flink list')
        x = jID.readlines()
        jobid = ''
        for i in x:
            if ('RUNNING' in i):
                jobid = i.split()[3]
                break
        os.system('flink cancel ' + jobid)
        # os.system('mv ./report/metrics/' + topic + '.csv ' + path + '/')
        perfValue = readCVS('./report/metrics/' + topic + '.csv')
        # print(perfValue)
        row.append(mbPreSecond)
        row.extend(perfValue)

        f1 = open('/home/gy/file/dataset/928/'+pro+'.csv', 'a+')
        cf1 = csv.writer(f1, lineterminator='\n')
        cf1.writerow(row)
        f1.close()
        os.system("rm ./report/metrics/*")
        os.system('sh ./deleteTopic.sh')
        time.sleep(30)


if __name__ == '__main__':
    pro='fixwindow'
    for i in range(50):
        os.system('stop-cluster.sh')
        confValue, record = makeRandomConfig()
        write2Configure(confValue)
        os.system("rm ~/flink-1.4.2/log/*")
        for i in ('172.16.1.2', '172.16.1.4', '172.16.1.10'):
            os.system('ssh -t -p 22 gy@' + i + ' "rm /home/gy/flink-1.4.2/log/*"')
            os.system('scp /home/gy/flink-1.4.2/conf/flink-conf.yaml gy@' + i + ':~/flink-1.4.2/conf/')
        os.system('start-cluster.sh')
        p = os.popen("sh ./isTask.sh")
        re = p.read()
        if re != '\n':
            if re.split()[-1] == 'TaskManager':
                # catchData(record)
                os.system('flink run ../flink-1.4.2/examples/streaming/Iteration.jar')
                dealData(record,pro)
        else:
            record.extend([float('inf'), float('inf')])
            f2 = open('/home/gy/file/dataset/928/'+pro+'.csv', 'a+')
            cf2 = csv.writer(f2, lineterminator='\n')
            cf2.writerow(record)
            f2.close()
        p.close()

