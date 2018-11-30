/*
 * Created by marcie on 18-11-29.
 */

#ifndef LIBPROCESS_START_DMIMEMINFO_H
#define LIBPROCESS_START_DMIMEMINFO_H

#include <signal.h>
#include <process/subprocess.hpp>
#include <process/io.hpp>
#include <stout/strings.hpp>

#include <hardware_resource.pb.h>

using process::subprocess;
using process::Subprocess;
using process::Future;

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::vector;

class MemoryCollector {
protected:
    /* the number of size,type,speed. */
    int num_size, num_type, num_speed;

    /* class ptr. */
    MemInfo *tmp;
public:
    /* message class. */
    MemoryCollection memoryCollection;

    /* storing strings during processing. */
    vector<string> tokens;

    /*
     * input command and get the returned information.
     */
    vector<string> get_dmiinfo_rows(){
        /* amount to input command and get the returned memory information. */
        Try<Subprocess> s = subprocess(
                "sudo -S dmidecode -t memory",
                Subprocess::FD(STDIN_FILENO),
                Subprocess::PIPE(),
                Subprocess::FD(STDERR_FILENO));
        Future<string> info = process::io::read(s.get().out().get());
        /* convert format to string. */
        string info_string = info.get();
        /* divide strings by "\n", that is divide it into rows. */
        tokens = strings::tokenize(info_string,"\n");
        return tokens;
    }

    /*
     *divide strings and filter out needed information
     * the return value type is DmiInfo (message calss).
     */
    MemoryCollection select_meminfo(vector<string> tokens){
        num_size = 0, num_type = 0, num_speed = 0;
        /* divide strings by ":", and filter memory information. */
        for (int i = 0; i < tokens.size(); i++) {
            vector<string> tokens_string = strings::tokenize(tokens[i],":");
            for (auto iter = tokens_string.begin(); iter != tokens_string.end(); iter++) {
                string nospace = strings::trim(*iter);
                if(nospace == "Maximum Capacity"){
                    iter++;
                    memoryCollection.set_max_size(strings::trim(*iter));
                }
                if(nospace == "Size"){
                    iter++;
                    if (strings::trim(*iter) != "No Module Installed") {
                        tmp = memoryCollection.add_mem_infos();
                        tmp->set_size(strings::trim(*iter));
                        num_size++;
                    }
                    iter--;
                }
                if(strings::trim(*iter) == "Type") {
                    iter++;
                    if(num_size > num_type) {
                        tmp->set_type(strings::trim(*iter));
                        num_type++;
                    }
                    iter--;
                }
                if(strings::trim(*iter) == "Speed") {
                    iter++;
                    if(strings::trim(*iter) != "Unknown") {
                        tmp->set_speed(strings::trim(*iter));
                        num_speed++;
                    }
                    iter--;
                }
            }
        }
        memoryCollection.set_device_quantity(num_size);
        return memoryCollection;
    }

    /*
     * output dmidecode infomation by protobuf message.
     */
    void show_meminfo(MemoryCollection memoryCollection){
        cout<<"************************************"<<endl;
        cout<<"Maximum Capacity："<<memoryCollection.max_size()<<endl;
        /* memoryCollection.info().size() */
        cout<<"当前机器有"<<memoryCollection.device_quantity()<<"个使用中的内存插槽"<<endl;
        cout<<"************************************"<<endl;
        for (auto iter = memoryCollection.mem_infos().begin(); iter != memoryCollection.mem_infos().end(); iter++) {
            cout<<"第"<<iter-memoryCollection.mem_infos().begin()+1<<"个Memory Device的信息为："<<endl;
            cout<<"Size:"<<iter.operator->()->size()<<endl;
            cout<<"Type:"<<iter.operator->()->type()<<endl;
            cout<<"Speed:"<<iter.operator->()->speed()<<endl;
            cout<<"************************************"<<endl;
        }
    }

    MemoryCollector(){

    }

    ~MemoryCollector(){

    }
};
#endif //LIBPROCESS_START_DMIMEMINFO_H