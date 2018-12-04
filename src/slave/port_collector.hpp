//
// Created by wqn on 18-12-4.
//

#ifndef CHAMELEON_PORT_COLLECTOR_HPP
#define CHAMELEON_PORT_COLLECTOR_HPP


#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

//#include <stout/strings.hpp>

#include <process/subprocess.hpp>
#include <process/future.hpp>
#include <process/io.hpp>

#include <hardware_resource.pb.h>

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::ifstream;
using process::Subprocess;
using process::Future;
namespace io = process::io;

namespace chameleon {
/*
 * className：PortCollector
 * date：18/12/03
 * author：Jessicallo sherlock_vip@163.com
 * description： Collecting Port range information by executing commands.
 */
    class PortCollector {
    private:
        string m_port_info;
        PortCollection* m_port_proto = new PortCollection();
    public:
        string get_port_string();
        PortCollection* split_port_string ();
    };
}


#endif //CHAMELEON_PORT_COLLECTOR_HPP
