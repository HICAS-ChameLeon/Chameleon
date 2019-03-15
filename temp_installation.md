#Docker_launch

##What you should do before "make"

###1.find the path of libmesos-1.3.2.so
```shell
#find the path
$locate libmesos-1.3.2.so
```

copy the path to Chameleon/src/slave/CMakeLists.txt : line194
```cmake
link_libraries(here is your libmesos-1.3.2.so path)
```

###2.modify mesos.hpp

modify the /usr/local/include/mesos/mesos.hpp : line23  

**change** 
```
#include <mesos/mesos.pb.h>
```  
**into**
```
#include <mesos.pb.h>
```

##then,you could compile the project

```
$cd build

$cmake ..

$make 
```

##How to launch a task by docker  
### Required Flags



| Flag     | Explanation |Example | 
| -------- | ----------- | ------ |
| --master | The master node pid. | --webui=172.20.110.59:6060 |
| --docker_image | The docker image name. | --docker_image="heldon/hello_docker"|
| --command | The docker container entry command. | --command= "java -jar /hello_docker.jar" |

###Optional Flags
|Flags|Explanation|Example|
|-----|-----------|-------|
|--cpu|Allocate how much Cpu per task.(default:1) | --cpu=0.5|
|--mem|Allocate memory(MB) per task.(default:32)|--mem=32|

first of all, ensure that master and slave node have been started  
then,  
**example:**
```shell
$cd build/src/slave

$./docker_launch --master=172.20.110.59:6060 --docker_image="heldon/hello_docker" --cpu=0.5 --command="java -jar /hello_docker.jar"
```
**Output:**
```Shell
Hello,Heldon!
```