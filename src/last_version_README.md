# Chameleon

## installation for Ubuntu 16.04


### First, make sure the following system requirements are statisfied.
```shell
# Update the packages.
$ sudo apt-get update

# install cmake
$ sudo apt-get install -y cmake

# Install a few utility tools.
$ sudo apt-get install -y tar wget git

# Install autotools
$ sudo apt-get install -y autoconf libtool automake

# Install the dependencies for libprocess.
$ sudo apt-get -y install build-essential libcurl4-nss-dev libsasl2-dev libsasl2-modules  libapr1-dev libsvn-dev zlib1g-dev iputils-ping
# Install the dependencies for hardware resources collection
if on x86_64 architecture
$ sudo apt-get -y install dmidecode lshw hdparm cpuid
if on Arm architecture
$ sudo apt-get -y install dmidecode lshw hdparm

```

### Second, clone the Chameleon git repository and build Chameleon.

```shell
$ git clone https://github.com/HICAS-ChameLeon/Chameleon.git

$ cd Chameleon 
$ mkdir build && cd build
$ cmake ..
$ make
```

## launch Chameleon in a cluster

### master
```shell
$ ./build/src/master/master 
```

### Optional Flags
| Flag | Explanation |example | 
| ------ | ------ | ------ |
| --port | master run on this port, (default 6060 ) | --port=6060 |
| --supermaster_path | the absolute path of supermaster executive, default("./super_master") | --supermaster_path=/home/lemaker/open-source/Chameleon/build/src/master/super_master|

### slave
```shell
$ ./build/src/slave/slave --master=172.20.110.228:6060
```

### Required Flags
| Flag | Explanation |example | 
| ------ | ------ | ------ |
| --master | the ip:port of master daemon | --master=172.20.110.228:6060 |

### Optional Flags

| Flag | Explanation |example | 
| ------ | ------ | ------ |
| --port | the port used by the slave daemon(default 6061) |--port=6061|
| --ht | the time interval of heartbeat that slave send a message to master(sec),and the ht must >= 2|--ht=300

### super_master

```shell
$./build/src/master/super_master
```

### Required Flags


| Flag | Explanation |example | 
| ------ | ------ | ------ |
| --master_path | the absolute path of master executive | --master_path=/home/lemaker/open-source/Chameleon/build/src/master/master |
| --initiator | the ip:port of the current master of first level or supermaster | --initiator=172.20.110.228:6060 |

#submitter

example

$./submitter  --master=172.20.110.228:6060  --port=6062 --path=/home/XXX/spark-2.3.0-bin-hadoop2.7.tgz

explain
  --port      the port used by the program
  --master    the master ip and port,example:127.0.0.1:8080
  --path      the path where the spark package exists









explain
  --port     the port used by the program(default 6060)
