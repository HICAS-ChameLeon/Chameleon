 #include <resource_collector/port_collector.hpp>

namespace chameleon{
/*
 * functionName：split_port_string
 * description：Separate and save the collected Port range information and encapsulate each information with protobuf
 * parameter: string m_port_info
 * returnValue: PortCollection*
 */
    PortCollection *PortCollector::split_port_string() {
        ifstream portfile("/proc/sys/net/ipv4/ip_local_port_range");
        if (portfile.fail())
            cout << "Read file failed" << endl;
        string line;
        std::getline(portfile, line);
        vector<string> ports = strings::split(line,
                                              "\t"); /*Divide Port range information from the command line by "\t" */
        PortInfo *portInfo = nullptr;
        portInfo = m_port_proto->add_port_infos();
        portInfo->set_firstport(ports[0]);
        portInfo->set_lastport(ports[1]);

        /*Assign value to 'port_quantity' attribute of class 'PortCollection'*/
        m_port_proto->set_port_quantity(m_port_proto->port_infos_size());

        return m_port_proto;

    }
}

