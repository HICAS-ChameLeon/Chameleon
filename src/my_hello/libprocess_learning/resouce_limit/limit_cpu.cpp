//
// Created by lilelr on 10/30/18.
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

    Try<Nothing> res= os::mkdir(path::join("/sys/fs/cgroup/cpu/", "lele_three"));
    if(res.isError()){
        cout<<"error occured when mkdir."<<endl;
        cout<<res.error()<<endl;
    }else{
        Try<Subprocess> s = subprocess("sh ./cpu.sh");
//        Try<Subprocess> s = subprocess("sleep 100");
        int pp_id = s.get().pid();
        Try<ProcessTree> tree = os::pstree(pp_id);

        const size_t total_children = tree.get().children.size();
        cout<<total_children<<endl;
        pid_t child = tree.get().process.pid;
        cout<<child<<endl;
        int grandchild = tree.get().children.front().process.pid;
        if(grandchild!=0){
            cout<<grandchild<<endl;

            // cgroup limit cpu
            Try<Subprocess> limit_cpu= subprocess("echo 50000 >  /sys/fs/cgroup/cpu/lele_three/cpu.cfs_quota_us");
            string write_task = "echo "+stringify(grandchild)+" > /sys/fs/cgroup/cpu/lele_three/tasks";
            Try<Subprocess> write_task_sub= subprocess(write_task);
            sleep(40);
            kill(grandchild,9);
        }else{
            cout<<child<<endl;

            // cgroup limit cpu
            Try<Subprocess> limit_cpu= subprocess("echo 50000 >  /sys/fs/cgroup/cpu/lele_three/cpu.cfs_quota_us");
            string write_task = "echo "+stringify(child)+" > /sys/fs/cgroup/cpu/lele_three/tasks";
            Try<Subprocess> write_task_sub= subprocess(write_task);
            sleep(40);
            kill(child,9);
        }

    }

    return 0;

}
