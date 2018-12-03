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
    Try<Subprocess> t = subprocess("su -");
    Try<Nothing> res = os::mkdir(path::join("/sys/fs/cgroup/cpu/", "cpu_test"));
    if (res.isError()) {
        cout << "error occured when mkdir." << endl;
        cout << res.error() << endl;
    } else {
        Try<Subprocess> limit_cpu = subprocess("echo 100000 > /sys/fs/cgroup/cpu/cpu_test/cpu.cfs_period_us");
        limit_cpu = subprocess("echo 10000 > /sys/fs/cgroup/cpu/cpu_test/cpu.cfs_quota_us");
        limit_cpu = subprocess("gcc /home/weiguow/下载/libprocess-master/my_hello/libprocess_learning/resouce_limit/cputime.c -o cputime");
        limit_cpu = subprocess("time ./cputime");
        sleep(4);
        limit_cpu = subprocess("time cgexec -g cpu:cpu_test ./cputime");
        sleep(20);
    }
    return 0;

}
