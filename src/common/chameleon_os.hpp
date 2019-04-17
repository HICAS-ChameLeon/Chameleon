/*
 * Copyright  ：SIAT 异构智能计算体系结构与系统研究中心
 * Author     ：Lele Li lilelr@163.com
 * Date       ：18-12-06
 * Description：common configurations for different operation systems.
 */

#ifndef CHAMELEON_CHAMELEON_OS_HPP
#define CHAMELEON_CHAMELEON_OS_HPP

#include <iostream>
#include <string>

#ifdef linux
#include <unistd.h>
#include <pwd.h>
#endif

#ifdef _WIN32
#include<Windows.h>
#endif

using std::string;
namespace chameleon {
    namespace setting {
        const string kx86Arch = "x86_64";
        const string kArmArch = "armv7l";
        const string MESOS_DIRECTORY = "/home/weiguow/project/chameleon/src/slave/mesos_executor/mesos-directory";
        const string MESOS_EXECUTOR_PATH = "/home/weiguow/project/chameleon/src/slave/mesos_executor/mesos-executor";
        const string FLAGS_LAUCHER_DIR = "/home/weiguow/project/mesos/mesos-1.3.2/build/src";
        static string SLAVE_EXE_DIR = "";
    }
//    namespace chameleon_os{
//        string get_current_user_name() {
//
//#if defined linux   //linux system
//            uid_t userid;
//    struct passwd* pwd;
//    userid=getuid();
//    pwd=getpwuid(userid);
//    return pwd->pw_name;
//
//#elif defined _WIN32  //windows system
//            const int MAX_LEN = 100;
//    char szBuffer[MAX_LEN];
//    DWORD len = MAX_LEN;
//    if( GetUserName(szBuffer, &len) )     //用户名保存在szBuffer中,len是用户名的长度
//        return szBuffer;
//
//#else  //outher system
//            return "";
//#endif
//
//
//        }
//    }



}


#endif //CHAMELEON_CHAMELEON_OS_HPP
