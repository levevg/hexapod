//
// Created by UnSeen on 23/12/2018.
//

#ifndef LIDARHEXAPOD_COMRECIEVER_H
#define LIDARHEXAPOD_COMRECIEVER_H


#include <thread>

extern std::thread hexapodComThread;
extern bool hexapodComThreadRun;
void hexapodComThreadFunc();

#endif //LIDARHEXAPOD_COMRECIEVER_H
