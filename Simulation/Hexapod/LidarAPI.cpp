//
// Created by UnSeen on 30/12/2018.
//

#include <iostream>
#include <math.h>
#include "LidarAPI.h"

void LidarAPI::sendCommand(LidarCommand command) {
    char cmd[9];
    cmd[0] = 0xA5;
    cmd[1] = command;
    if (command == lcExpressScan) {
        cmd[2] = 0x05;
        cmd[3] = 0x00;
        cmd[4] = 0x00;
        cmd[5] = 0x00;
        cmd[6] = 0x00;
        cmd[7] = 0x00;
        cmd[8] = 0x22;
        _port->send(cmd, 9);
    } else {
        _port->send(cmd, 2);
    }
}

void LidarAPI::waitForScanStart() {
    uint8_t cmd[7];
    cmd[0] = 0xA5;
    cmd[1] = 0x5A;
    cmd[2] = 0x05;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = 0x40;
    cmd[6] = 0x82;
    uint8_t state = 0;

    uint8_t b = 0;
    while (true) {
        if (_port->readChar(b)) {
            if (cmd[state] == b) {
                state += 1;
                if (state == 7) {
                    _scanning = true;
                    break;
                }
            } else {
                state = 0;
            }
        }
    }
}

LidarSample LidarAPI::readSample() {
    LidarSample sample{};

    uint8_t s[5];
    int i = 0;
    while (i < 5) {
        if (_port->readChar(s[i])) ++i;
    }

    sample.quality = s[0];
    sample.angle = (90 - ((s[1] >> 1) + (s[2] << 7)) / 64.0) / 180 * M_PI;
    sample.distance = 0.25 * (s[3] + (s[4] << 8));

    return sample;
}

std::vector<LidarSample> LidarAPI::readFullCircle() {
    std::vector<LidarSample> circle;
    if (prevFirst.quality & 1) {
        circle.emplace_back(prevFirst);
    }
    do {
        prevFirst = readSample();
        if (prevFirst.quality & 2) {
            circle.emplace_back(prevFirst);
        } else {
            break;
        }
    } while (true);
    return circle;
}
