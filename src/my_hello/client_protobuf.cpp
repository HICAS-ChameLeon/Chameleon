#include <iostream>
#include <sstream>
#include <fstream>

#include <stout/os.hpp>
#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>

#include "Offer.pb.h"

using namespace process;

using namespace process::http;

using std::cerr;
using std::cout;
using std::endl;

using std::chrono::seconds;

using process::Future;
using process::Promise;

using process::http::Request;
using process::http::OK;
using process::http::InternalServerError;

using std::string;

using namespace std;
class Client : public ProtobufProcess<Client> {

public:

    UPID server;

    Client() : ProcessBase("my_client") {  //注册myclient
    }


    virtual void initialize() {
//     route("/vars", &MyProcess::vars);
        route("/vars", "hello ", [=](const Request &request) {
            string body = "... vars here ...";
            OK response;
            response.headers["Content-Type"] = "text/plain";
            std::ostringstream out;
            out << body.size();
            response.headers["Content-Length"] = out.str();
            response.body = body;
            return response;
        });

//     install("stop", &MyProcess::stop);
        install("stop", [=](const UPID &from, const string &body) {
            terminate(self());
        });

        install<Offer>(&Client::report_from_server, &Offer::key);



//        cout<<"pinf"<<endl;

    }

    void report_from_server(const string &key) {
        cout << "report from server" << endl;
        cout << key << endl;
    }

    void send_server_a_message() {
        Offer k;
        string client_id = this->self();
        string str_file =  test_read_a_file();
        k.set_key(client_id);

        k.set_value(str_file);
//        k.set_value("leoox");
        k.set_lele_label("OS:linux");
        send(server, k);
    }

    void String2Image(string binFile,const char* outImage)
    {
        fstream imgFile(outImage,ios::binary|ios::out);

        for(int i=0; i<binFile.length(); ++i)
        {
            imgFile << binFile[i];
        }

        imgFile.close();
    }

    string test_read_a_file(){
        string str_f;
//        ifstream myfile("/home/lilelr/open-source/PeparationLearning/src/li_le_lr/knowledge/iostream/code.txt",ios_base::in);
//        ifstream myfile("/home/lilelr/open-source/libprocess-start/my_hello/proto/program/hello_world_main",ios_base::in);
//        ifstream myfile("/home/lilelr/open-source/libprocess-start/my_hello/proto/program/java-hello-thread.jar",ios_base::in);
        ifstream myfile("/home/weiguow/hello.py",ios_base::in);
        if(myfile.fail())
        {
            cout<<"文件读取失败或指定文件不存在!"<<endl;
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
                cout<<"文件内容已经全部读完"<<endl;
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


};

int main() {

//    string str;
//    client.test_read_a_file(str);
//    // 绑定 IP
//    os::setenv("LIBPROCESS_IP","10.211.55.4");
    // 绑定端口
    os::setenv("LIBPROCESS_PORT", stringify(5051));
    process::initialize("slave");
    Client client;

    PID<Client> cur_client = process::spawn(client);
    cout << endl;
    cout << "Running client on " << process::address().ip << ":" << process::address().port << endl;  //显示ip地址和端口
    cout << "PID" << endl;
    cout << "please input the master UPID:" << endl;

    string master_str;  //
    cin >> master_str;
    UPID masterUPID(master_str);
    client.server = masterUPID;
    client.send_server_a_message();

    process::wait(client.self());
//    const PID<Master> masterPid = master.self();
//    cout<<masterUPID<<endl;
//    Client client;
//    process:PID<Master> cur_master =masterUPID;
//    send(masterUPID, "ping");
//    PID pid()

//    PID<Master> masterUPID("my_master@10.211.55.4:33840");
//    process::dispatch(masterUPID,&Master::report,k.key());
//    process::dispatch(pid,&Master::report,k.key());
    return 0;
}
