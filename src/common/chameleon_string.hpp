/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-12-03
 * Description：common operations for string
 */

#ifndef CHAMELEON_CHAMELEON_STRING_HPP
#define CHAMELEON_CHAMELEON_STRING_HPP

#include <cstring>
#include <string>

using std::string;
namespace chameleon {
    inline char* string_to_char(string& input){
        const char* constc = nullptr;
        constc = input.c_str();
        char* output = nullptr;
        output = const_cast<char*>(constc);
        return output;
    }
    
    /**
     * construct a  UPID string.
     * For example, construct_UPID_string("master", "127.0.0.1","6061") returns "master@127.0.0.1:6061"
     * @param prefix 
     * @param ip 
     * @param port 
     * @return  a  UPID string
     */
    inline string construct_UPID_string(const string prefix,const string ip, const string port ){
            string result = prefix;
            result.append("@");
            result.append(ip);
            result.append(":");
            result.append(port);
            return result;
    }
}

#endif //CHAMELEON_CHAMELEON_STRING_HPP
