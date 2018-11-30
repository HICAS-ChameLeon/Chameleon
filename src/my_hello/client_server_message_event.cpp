#include <iostream>
#include <sstream>

#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>
using namespace process;
;
using namespace process::http;
using process::MessageEvent;
using process::PID;
using process::Process;
using process::spawn;
using process::terminate;
using process::wait;
using namespace std;

class ClientProcess : public Process<ClientProcess>
{
public:
    ClientProcess(const UPID& server) : server(server) {}
    UPID server;
    void initialize() override
    {
        send(server, "ping");
    }

    void visit(const MessageEvent& event) override
    {
        if (event.message->from == server &&
            event.message->name == "pong") {
            std::cout<<"terminde client after receiving pong from server"<<std::endl;
            terminate(self());
        }
    }
};

class ServerProcess : public Process<ServerProcess>
{
public:
protected:
    void visit(const MessageEvent& event) override
    {
        if (event.message->name == "ping") {
            send(event.message->from, "pong");
        }
        std::cout<<"terminde server after sending ping to server"<<std::endl;

        terminate(self());
    }
};

int main(int argc, char** argv)
{
    PID<ServerProcess> server = spawn(new ServerProcess(), true);
    PID<ClientProcess> client = spawn(new ClientProcess(server), true);

    wait(server);
    wait(client);

    return 0;
}