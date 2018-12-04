#include <port_collector.hpp>

namespace chameleon{

///* Test: one
// * functionName：get_port_string
// * description：Executing commands to obtain Port range information in the form of string
// * parameter: none
// * returnValue: Port range information in the form of string
// */
//   string PortCollector::get_port_string() {
//       Try<Subprocess> s = subprocess(
//               "cat /proc/sys/net/ipv4/ip_local_port_range ",
//               Subprocess::FD(STDIN_FILENO),
//               Subprocess::PIPE(),
//               Subprocess::FD(STDERR_FILENO));
//        Future<string> info = io::read(s.get().out().get());
//        m_port_info = info.get();
//        return m_port_info;
//   }
//
///*
// * functionName：get_port_string
// * description：Separate and save the collected Port range information and encapsulate each information with protobuf
// * parameter: string m_port_info
// * returnValue: PortCollection*
// */
//    PortCollection *PortCollector::split_port_string() {
//        get_port_string();
//        vector<string> ports = strings::split(m_port_info, "\t");  /*Divide Port range information from the command line by "\t" */
//        PortInfo *portInfo = nullptr;
//        portInfo =m_port_proto->add_port_infos();
//        portInfo->set_firstport(ports[0]);
//        ports[1].pop_back();                 /*Remove the last newline '\n'*/
//        portInfo->set_lastport(ports[1]);
//
//        /*Assign value to 'gpu_quantity' attribute of class 'GPUCollection'*/
//        m_port_proto->set_port_quantity(m_port_proto->port_infos_size());
//
//        return m_port_proto;
//
//    }

    /*Test: two
    */
    PortCollection *PortCollector::split_port_string() {
        ifstream portfile("/proc/sys/net/ipv4/ip_local_port_range");
        if(portfile.fail())
            cout << "Read file failed" <<endl;
        string line;
        std::getline(portfile,line);
        vector<string> ports = strings::split(line, "\t"); /*Divide Port range information from the command line by "\t" */
        PortInfo *portInfo = nullptr;
        portInfo =m_port_proto->add_port_infos();
        portInfo->set_firstport(ports[0]);
        portInfo->set_lastport(ports[1]);

        /*Assign value to 'gpu_quantity' attribute of class 'GPUCollection'*/
        m_port_proto->set_port_quantity(m_port_proto->port_infos_size());

        return m_port_proto;

    }
}
