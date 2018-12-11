// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <stout/os.hpp>
#ifndef __WINDOWS__
#include <stout/os/fork.hpp>
#endif // __WINDOWS__
#include <stout/os/pstree.hpp>


#include <process/defer.hpp>
#include <process/dispatch.hpp>
#include <process/future.hpp>
#include <process/http.hpp>
#include <process/process.hpp>
#include <process/protobuf.hpp>

#include <Teacher.pb.h>

using namespace process;

using namespace process::http;

#ifndef __WINDOWS__
using os::Exec;
using os::Fork;
#endif // __WINDOWS__
using os::Process;
using os::ProcessTree;

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
using std::shared_ptr;
using std::make_shared;

class StudentProcess : public ProtobufProcess<StudentProcess> {

public:
    StudentProcess() : ProcessBase("student") {
    }


    virtual void initialize() {

        route(
                "/add",
                "Adds the two query arguments",
                [](Request request) {
//                int a = numify<int>(request["a"]).get();
//                int b = numify<int>(request["b"]).get();
                    int a = 3;
                    int b = 4;
                    std::ostringstream result;
                    result << "{ \"result\": " << a + b << "}";
                    JSON::Value body = JSON::parse(result.str()).get();
                    return OK(body);
                });

//     route("/vars", &MyProcess::vars);
//        route("/vars", "hello ", [=]( Request &request) {
//            string body = "... vars here ...";
//            OK response;
//            response.headers["Content-Type"] = "text/plain";
//            std::ostringstream out;
//            out << body.size();
//            response.headers["Content-Length"] = out.str();
//            response.body = body;
//            return response;
//        });

//     install("stop", &MyProcess::stop);
        install("stop", [=](const UPID &from, const string &body) {
            terminate(self());
        });
//
        install<Teacher>(&StudentProcess::report_from_teacher, &Teacher::chinese);

//        install<Teacher>(&StudentProcess::report_from_teacher2, &Teacher::);
    }

    void report_from_teacher(const Chinese& c) {
        cout << "entering into report_from_teacher" << endl;
        cout<< c.age()<<endl;
        cout<<c.name()<<endl;

    }

    void report_from_teacher2(const Teacher& age) {
        cout << "entering into report_from_teacher2" << endl;
//        cout<< c.age()<<endl;
//        cout<<c.name()<<endl;
//        cout<<age<<endl;

    }

private:
//    shared_ptr<Teacher>
};

int main() {

    StudentProcess student_pro;
    PID<StudentProcess> cur_student = process::spawn(student_pro);
    cout << "Running student on " << process::address().ip << ":" << process::address().port << endl;
    cout << "PID" << endl;

    const PID<StudentProcess> studentPid = student_pro.self();
    cout << studentPid << endl;
//   string a =  getenv("LIBPROCESS_PORT");
//    cout<<a<<endl;
//    process::dispatch(masterPid,&Master::report,k.key());
    process::wait(student_pro.self());


    return 0;
}