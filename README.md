# Chameleon 

## installation for Ubuntu 16.04


First,
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
$ sudo apt-get -y install dmidecode lshw hdparm cpuid
```

Second,
# How to run the program

#.submitter
$.example
  ./submitter  --masterinfo=172.20.110.228:6060  --rport=6062 --spath=/home/XXX/spark-2.3.0-bin-hadoop2.7.tgz

$.explain
  --masterinfo=VALUE     master ip and port
  --rport=VALUE          submitter run port
  --spath=VALUE          spark exists path


#slave
$.example
 ./slave  --masterinfo=172.20.110.228:6060 --slaveport=6061

$.explain
  --masterinfo=VALUE     master_ip_and_port
  --slaveport=VALUE      slave_port (defalut 0)

#master
$.example
 ./master --mport=6060

$.explain
  --mport=VALUE     master run port
