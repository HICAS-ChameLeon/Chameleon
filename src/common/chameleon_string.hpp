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
}

#endif //CHAMELEON_CHAMELEON_STRING_HPP
