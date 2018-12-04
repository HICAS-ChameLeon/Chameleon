//
// Created by lemaker on 18-12-4.
//

#ifndef CHAMELEON_CHAMELEON_FILE_HPP
#define CHAMELEON_CHAMELEON_FILE_HPP
#include <cstring>
#include <string>
#include <fstream>

#include <glog/logging.h>

using std::string;

namespace chameleon {
    /**
     * reads a file and saves its bytes to a string
     * @param file_path
     * @return  a string contained the bytes of the file
     */
    string read_a_file(const string& file_path){
        string str_f;
//        ifstream myfile("/home/lilelr/open-source/PeparationLearning/src/li_le_lr/knowledge/iostream/code.txt",ios_base::in);
//        ifstream myfile("/home/lilelr/open-source/libprocess-start/my_hello/proto/program/hello_world_main",ios_base::in);
//        ifstream myfile("/home/lilelr/open-source/libprocess-start/my_hello/proto/program/java-hello-thread.jar",ios_base::in);
        std::ifstream myfile(file_path,std::ios_base::in);
        if(myfile.fail())
        {
            LOG(ERROR)<<"文件读取失败或指定文件不存在!";
        }
        else
        {
            char ch;
            while(myfile.get(ch))
            {
                str_f+=ch;
//                cout<<ch;
//                cout.flush();
            }
            if(myfile.eof())
            {
                LOG(INFO)<<"文件内容已经全部读完";
                myfile.close();
            }
//            while(myfile.get(ch))
//            {
//                cout<<ch;
//            }

        }
//        cout<<str_f<<endl;
        return str_f;
//        system("pause");
    }
}


#endif //CHAMELEON_CHAMELEON_FILE_HPP
