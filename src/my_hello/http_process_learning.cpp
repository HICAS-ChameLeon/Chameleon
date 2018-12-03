#include <iostream>
#include <sstream>
#include <string>

#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>
#include <process/socket.hpp>
#include <process/gtest.hpp>


using namespace process;
using namespace process::http;

using process::http::URL;

using process::network::inet::Address;
using process::network::inet::Socket;

class HttpProcess:public Process<HttpProcess>
{
protected:
    virtual void initialize(){
        route("/testing", None(), [](const Request request){
            std::string body = "... vars here ...";
            OK response;
            std::cout<<"hello initialize"<<std::endl;
            response.headers["Content-Type"] = "text/plain";
            std::ostringstream out;
            out << body.size();
            response.headers["Content-Length"] = out.str();
            response.body = body;
            return response;
        });
    }
};

class Http{


public:
    Owned<HttpProcess> process;

    Http():process(new HttpProcess()){
        process:PID<HttpProcess> curr_http= spawn(process.get());
        std::cout<<process.get()->self().address.ip <<process.get()->self().address.port<<std::endl;
        std::cout<<address().ip<<":"<<address().port<<std::endl;
    }

    virtual ~Http(){
        terminate(process.get());
        wait(process.get());
    }
};

int main(){
//    Http http;
//    while (true){
//
//    }
//    HttpProcess my_http_process;
//    spawn(my_http_process);
//    std::cout<<address().ip<<":"<<address().port<<std::endl;
//    process::wait(my_http_process.self());

    Http http;
   Try<Socket> create = Socket::create();
    Socket socket = create.get();
    socket.connect(http.process->self().address);
    std::cout<<http.process->self().address<<std::endl;
    socket.connect(http.process->self().address);
    std::ostringstream out;
    out << "GET /" << http.process->self().id << "/body"
        << " HTTP/1.0\r\n"
        << "Connection: Keep-Alive\r\n"
        << "\r\n";

    const std::string data = out.str();
    socket.send(data);
    std::string response = "HTTP/1.1 200 OK";
    Future<std::string> response2 = socket.recv(response.size());
    std::cout<<response2.get()<<std::endl;
//    std::cout<<data<<std::endl;


    return 0;

}