//
// Created by lilelr on 10/29/18.
//
#include <sys/types.h>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <iostream>

#include <stout/try.hpp>
#include <stout/os/mkdir.hpp>
#include <process/gmock.hpp>
#include <process/gtest.hpp>

#include <stout/os.hpp>
#include <stout/os/pstree.hpp>


#include <process/subprocess.hpp>


using process::Subprocess;
using process::subprocess;
using os::ProcessTree;


using std::string;
using namespace std;

int main(){

//    Try<Subprocess> s = subprocess("sleep 50");
////
//    int pp_id = s.get().pid();
//    cout<<pp_id<<endl;
//    Try<ProcessTree> tree = os::pstree(pp_id);
//    const size_t total_children = tree.get().children.size();
//    cout<<total_children<<endl;
//    pid_t child = tree.get().process.pid;
//    cout<<child<<endl;
//    pid_t grandchild = tree.get().children.front().process.pid;
//    cout<<grandchild<<endl;


    Try<Nothing> res= os::mkdir(path::join("/sys/fs/cgroup/blkio/", "lele_three"));
    if(res.isError()){
        cout<<"error occured when mkdir."<<endl;
        cout<<res.error()<<endl;
    }else{
        Try<Subprocess> s = subprocess("dd if=/dev/sda of=/dev/null");
//        Try<Subprocess> s = subprocess("sleep 100");
        int pp_id = s.get().pid();
        Try<ProcessTree> tree = os::pstree(pp_id);

        const size_t total_children = tree.get().children.size();
        cout<<total_children<<endl;
        pid_t child = tree.get().process.pid;
        cout<<child<<endl;
        int grandchild = tree.get().children.front().process.pid;
        if(grandchild!=0){
            // cgroup limit disk I/O
            Try<Subprocess> limit_disk= subprocess("echo '8:0   1048576' >  /sys/fs/cgroup/blkio/lele_three/blkio.throttle.read_bps_device");
            string write_task = "echo "+stringify(grandchild)+" > /sys/fs/cgroup/blkio/lele_three/tasks";
            Try<Subprocess> write_task_sub= subprocess(write_task);
        }

        cout<<grandchild<<endl;
//
    }

    return 0;

}
