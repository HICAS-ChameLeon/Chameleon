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
using namespace std;

class TeacherProcess : public ProtobufProcess<TeacherProcess> {

public:
    TeacherProcess() : ProcessBase("teacher") {
    }


    virtual void initialize() {

////route("/add",)
//        route("/add", "Adds the two query arguments", [](Request request) {
//            int a = 3;
//            int b = 4;
//            std::ostringstream result;
//            result << " { " << "\"result\":" << a + b << "}";
//            JSON::Value body = JSON::parse(result.str()).get();
//            return OK(body);
//        });
//
////route("/vars", &MyProcess::vars);
//        route("/vars", "hello ", [=](const Request &request) {
//            string body = "... vars here ...";
//            OK response;
//            response.headers["Content-Type"] = "text/plain";
//            std::ostringstream out;
//            out << body.size();
//            response.headers["Content-Length"] = out.str();
//            response.body = body;
//            return response;
//        });

//install("stop", &MyProcess::stop);
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
//        install<Offer>(&Master::report_from_client, &Offer::key,&Offer::value);
    }

    void report_from_student(const string &key,const string& value) {
        cout << "entering into report" << endl;
        UPID clientUPID(key);
    }

    void send_to_student(UPID student_upid, Teacher& t){
        send(student_upid,t); // 调用礼拜process的API 发送消息
    }
};

int main() {
    Teacher t;
    Chinese c;
    c.set_name("teacher_li");
    c.set_career("I am a teacher");
    c.set_id(1);
    c.set_age(26);
    CHECK(!t.has_chinese());
    t.set_allocated_chinese(&c);
    CHECK(t.has_chinese());

    TeacherProcess teacher_pro;

    PID<TeacherProcess> cur_teacher = process::spawn(teacher_pro);
    cout << "Running teacher on " << process::address().ip << ":" << process::address().port << endl;
    cout << "PID" << endl;

    const PID<TeacherProcess> teacherPid = teacher_pro.self();
    cout << teacherPid << endl;

    cout << "please input the student UPID:" << endl;

    string student_upid;  //
    cin >> student_upid;
    UPID studentUPID(student_upid);
    teacher_pro.send_to_student(student_upid,t);

    process::wait(teacher_pro.self());

    return 0;
}