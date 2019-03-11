### coding: utf-8
from __future__ import print_function
from __future__ import division
import traceback, os, threading, time, multiprocessing

try:#python3
     from io import BytesIO
     import urllib.request
except ImportError:#python2
     import StringIO
     import urllib

import pycurl
import json, re

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
    if html:
        hjson = json.loads(html)
    else:
        hjson = {}

    c.close()
    b.close()
    return hjson

'''
    Json data conversion
    :parameter  data：data in any format
    :return     data in json format
'''
def change_data(data):
    try:
        return json.dumps(data, separators=[',', ':'], sort_keys=True)
    except Exception as e:
        print(e)
        return None

def getFlinkJobid():
    jobinfo = handle_html('http://172.20.110.100:8081/joboverview/running')
    joid = jobinfo["jobs"]
    if joid:
        joid = joid[-1]["jid"]
    else:
        joid = ""
    return joid

def getJobStatus():
    jobinfo = handle_html('http://172.20.110.100:8081/joboverview/running')
    joid = jobinfo["jobs"]
    if joid:
        joid = joid[-1]["jid"]
    else:
        joid = ""
    return joid


def getFlinkFinishJobid():
    jobinfo = handle_html('http://172.20.110.100:8081/joboverview/running')
    joid = jobinfo["jobs"]
    if joid:
        joid = joid[-1]["jid"]
    else:
        joid = ""
    return joid

def cancelJob():
    jID = os.popen("ssh zyx@172.20.110.100 '/home/zyx/open-source/flink-1.4.2/build-target/bin/flink list'")
    x = jID.readlines()
    if x:
        for i in x:
            if ('mlass-compute-tp-min' in i):
                jobid = i.split()[3]
                os.system("ssh zyx@172.20.110.100 '/home/zyx/open-source/flink-1.4.2/build-target/bin/flink cancel " + jobid + "'")

def getPerformanceMetrics(jobid):
    jobinfo = handle_html('http://172.20.110.100:8081/jobs/' + str(jobid))
    '''Information about vertices'''
    p99_latency = 0.0
    if jobinfo:
        vertices = jobinfo['vertices']
        if vertices:
            for i in range(5):
                lmetric = 0.0
                for vertex in vertices:
                    vertexId = vertex['id']
                    print(vertexId)
                    temp = handle_html('http://localhost:8081/jobs/' + str(jobid) + '/vertices/' + str(vertexId) + '/metrics?get='
                                  '0.Sink__Unnamed.latency')
                    # if temp is None:
                    #     temp = handle_html('http://localhost:8081/jobs/' + str(jobid) + '/vertices/' + str(vertexId) + '/metrics?get='
                    #               '1.Sink__Unnamed.latency')
                    print(temp)
                    for met in temp:
                        value = met["value"]
                        if value != "":
                            s = value.find("p99")
                            reg = '^\d+\.\d+$'
                            patternIntOrFloat = re.compile(reg)
                            if (s != -1):
                                latency = value[s:].replace(" ", "").split(",")
                                p99 = latency[0].split("=")[1]
                                lmetric += float(p99)
                            elif re.search(patternIntOrFloat, value):
                                lmetric = lmetric + float(value)
                if abs(p99_latency-lmetric) > 1.0e-3:
                    p99_latency = lmetric
                    break
                # print("latency is: " + str(p99_latency))
        else:
            p99_latency = p99_latency + 0.0
    else:
        p99_latency = 1000001.0
    return p99_latency



def getFlinkHome():
    with open('./conf.conf', 'r') as file_to_read:
        lines = file_to_read.readlines()
        flinkHome = lines[0].split()[1]
    return flinkHome

def getSlaves():
    slaves=[]
    with open('./conf.conf', 'r') as file_to_read:
        lines = file_to_read.readlines()
        for line in lines:
            if (line == '\n' or line[0] == '#'):
                pass
            else:
                i = line.split(',')
                ##### remove '\r\n'
                i[-1] = i[-1].split('\n')[0]
                slaves.extend(i)
    return slaves
