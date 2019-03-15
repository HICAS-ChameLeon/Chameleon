#Docker_launch

##what you should do before "make"

###1.find the path of libmesos-3.1.2.so
```shell
#find the path
$locate libmesos-3.1.2.so
```

copy the path to Chameleon/src/slave/CMakeLists.txt : line194
```cmake
link_libraries(here is your libmesos-3.1.2.so path)
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