//
// Created by UnSeen on 23/12/2018.
//

#include "ComReciever.h"
#include "Utils.h"
#include "FirmwareCode/Serial.h"
#include "FirmwareCode/Config.h"
#include <iostream>

std::thread hexapodComThread;
bool hexapodComThreadRun = true;

typedef enum {
    csWaitCmd,
    csWaitPositionHigh,
    csWaitPositionLow,
    csWaitDurationHigh,
    csWaitDurationLow
} ComState;

typedef struct {
    uint8_t jointId;
    uint16_t position;
} MoveCmd;

std::vector<MoveCmd> commands;

void executeCommands(uint16_t duration) {
    for (auto cmd: commands) {
        Joint *joint = hexapod.joints[cmd.jointId];
        if (joint->id() == cmd.jointId) {
            joint->movingStartTime = Utils::time();
            joint->movingEndTime = joint->movingStartTime + 0.001*duration;
            joint->movingStartPosition = joint->currentPosition;
            joint->movingEndPosition = (cmd.position - joint->offset) / joint->span;
        }
    }
    commands.clear();
}

void hexapodComThreadFunc() {
    ComState state = csWaitCmd;
    uint8_t b = 0, jointId = 0;
    uint16_t position = 0, duration = 0;
    while (hexapodComThreadRun) {
        if (servosPort.readChar(b)) {
            switch (state) {
                case csWaitCmd: {
                    if (b == 0xA1) {
                        state = csWaitDurationHigh;
                    } else
                    if (b >= 0x80 && b <= 0x80 + 32) {
                        jointId = b - 0x80;
                        state = csWaitPositionHigh;
                    }
                break;
                }
                case csWaitPositionHigh: {
                    if (b > 0x0F) {
                        state = csWaitCmd;
                        break;
                    }
                    position = b << 8;
                    state = csWaitPositionLow;
                    break;
                }
                case csWaitPositionLow: {
                    position += b;
                    commands.emplace_back(MoveCmd{jointId, position});
                    state = csWaitCmd;
                    break;
                }
                case csWaitDurationHigh: {
                    duration = b << 8;
                    state = csWaitDurationLow;
                    break;
                }
                case csWaitDurationLow: {
                    duration += b;
                    executeCommands(duration);
                    state = csWaitCmd;
                    break;
                }
            }
        }
    }
}
