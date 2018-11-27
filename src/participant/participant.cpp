//
// Created by lemaker on 18-11-26.
//

#include "participant.hpp"
using namespace chameleon;

int main(){
    os::setenv("LIBPROCESS_PORT", stringify(5051));
    process::initialize("participant");

    Participant participant;
    PID<Participant> cur_participant = process::spawn(participant);
    cout << "Running participant on " << process::address().ip << ":" << process::address().port << endl;
    cout << "PID" << endl;

    const PID<Participant> participant_pid = participant.self();
    cout << participant_pid << endl;
    process::wait(participant.self());
    return 0;
}