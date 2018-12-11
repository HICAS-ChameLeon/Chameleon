//
// Created by weiguow on 18-11-2.
//
//
// Created by lilelr on 10/30/18.
//
#include <sys/types.h>

#include <memory>
#include <string>
#include <iostream>

#include <stout/try.hpp>
#include <stout/os/mkdir.hpp>

#include <stout/os.hpp>
#include <stout/os/pstree.hpp>

#include <process/subprocess.hpp>


using process::Subprocess;
using process::subprocess;
using os::ProcessTree;


using std::string;
using namespace std;


int main() {
    //创建文件夹
    Try<Nothing> res = os::mkdir(path::join("/sys/fs/cgroup/memory/", "nick_memory"));

    //如果创建失败
    if (res.isError()) {
        cout << "error occured when mkdir." << endl;
        cout << res.error() << endl;
    } else {
        Try<Subprocess> limit_cpu_order_0 = subprocess("echo 314572800 >  /sys/fs/cgroup/memory/nick_memory/memory.limit_in_bytes");
        limit_cpu_order_0 = subprocess("echo 0 > /sys/fs/cgroup/memory/nick_memory/memory.swappiness");
        limit_cpu_order_0 = subprocess("gcc /home/weiguow/project/C++/libprocess-start/my_hello/libprocess_learning/resouce_limit/weiguow_mem.c -o mem");
        limit_cpu_order_0 = subprocess("./mem");
        //等待两秒，让mem程序完成创建使用五个内存,否则进程会被下一条命令占用，两者处于并行状态
        sleep(2);
        limit_cpu_order_0 = subprocess("cgexec -g memory:nick_memory ./mem");
        sleep(10);
        }
    return 0;
    }




