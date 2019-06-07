//
// Created by root on 19-5-23.
//
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stout/strings.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <process/subprocess.hpp>
#include <process/process.hpp>


using namespace std;
//
//从文件读入到string里
string readFileIntoString()
{
    std::ifstream file("/home/zyx/CLionProjects/Chameleon/src/configure/hm_ga/DAC/autoDAC.py");
    ostringstream buf;
    char ch;
    while(buf&&file.get(ch))
        buf.put(ch);
    return buf.str();
}

int writeStringIntoFile(const std::string & file_string, const std::string str )
{
    std::ofstream OsWrite(file_string,std::ofstream::app);
    OsWrite<<str;
    OsWrite<<std::endl;
    OsWrite.close();
    return 0;
}

void execPrepare(string p_path){
    Try<process::Subprocess> python = subprocess(
            p_path,
            process::Subprocess::FD(STDIN_FILENO),
            process::Subprocess::FD(STDOUT_FILENO),
            process::Subprocess::FD(STDERR_FILENO)
    );
}

void executeDAC(const string file_path){
    string command = "python " + file_path;
    Try<process::Subprocess> python = subprocess(
            command,
            process::Subprocess::FD(STDIN_FILENO),
            process::Subprocess::FD(STDOUT_FILENO),
            process::Subprocess::FD(STDERR_FILENO)
    );

}

int main() {

    string str = readFileIntoString();
    const string cout;
   // std::cout << str << std::endl;
    string user = "zyx";
    string report = "/home/zyx/app/Hibench-7.0/report";
    string run = "/home/zyx/app/Hibench-7.0/bin/workloads/micro/wordcount/spark";
    string name = "wordcount";
  //  EXPECT_EQ("hello*", strings::replace("hello/", "/", "*"));
    string str1 = strings::replace(str, "{user}", user);
    string str2 = strings::replace(str1, "{report}", report);
    string str3 = strings::replace(str2, "{run.sh}", run);
    std::cout << str3 << std::endl;

    string file_path = "/home/zyx/CLionProjects/Chameleon/src/configure/hm_ga/DAC/" + name + "_DAC.py" ;
    writeStringIntoFile(file_path,str3);

    bool isPrepare = true;
    string p_path = "/home/zyx/app/Hibench-7.0/bin/workloads/micro/wordcount/prepare/prepare.sh";
    if(isPrepare){
        //execPrepare(p_path);
    }
//    executeDAC(file_path);
}



