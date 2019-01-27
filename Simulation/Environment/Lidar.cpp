//
// Created by UnSeen on 30/12/2018.
//

#include <thread>
#include <algorithm>
#include "Lidar.h"

Lidar::Lidar(Serial *port) {
    threadsRun = true;
    _port = port;
    std::thread readThread(&Lidar::serialReadThreadFunc, this);
    readThread.detach();
    std::thread writeThread(&Lidar::sendInBackgroundThreadFunc, this);
    writeThread.detach();
}

void Lidar::serialReadThreadFunc() {
    enum {
        stWaitA5,
        stWaitCmd,
        stWaitPayloadSize,
        stWaitNextPayloadByte,
        stWaitChecksum,
    } cmdState;
    cmdState = stWaitA5;

    uint8_t b;
    LidarCommand command = lcNone;

    while (threadsRun) {
        if (_port->readChar(b)) {
            OutputDebugStr(std::to_string(b).c_str());
            switch (cmdState) {
                case stWaitA5:
                    if (b == 0xA5) {
                        cmdState = stWaitCmd;
                    }
                    break;
                case stWaitCmd:
                    if (b == lcScan || b == lcExpressScan || b == lcForceScan || b == lcGetHealth ||
                            b == lcGetInfo || b == lcGetSampleRate || b == lcReset || b == lcStop) {
                        command = static_cast<LidarCommand>(b);
                        if (command == lcExpressScan) {
                            cmdState = stWaitPayloadSize;
                            break;
                        } else {
                            state = lsRequestProcessing;
                            gotCommand(command);
                            if (state == lsRequestProcessing) {
                                state = lsIdle;
                            }
                        }
                    }
                    cmdState = stWaitA5;
                    break;
                case stWaitPayloadSize:
                    cmdState = stWaitA5; // not implemented
                    break;
                case stWaitNextPayloadByte:
                    cmdState = stWaitA5; // not implemented
                    break;
                case stWaitChecksum:
                    cmdState = stWaitA5; // not implemented
                    break;
            }
        }
    }
}

void Lidar::gotCommand(LidarCommand command) {
    switch (command) {
        case lcNone:
            break;
        case lcStop:
            if (state == lsScanning) {
                state = lsIdle;
                break;
            }
            break;
        case lcReset:
            state = lsIdle;
            break;
        case lcScan: case lcForceScan:
            state = lsScanning;
            uint8_t cmd[7];
            cmd[0] = 0xA5;
            cmd[1] = 0x5A;
            cmd[2] = 0x05;
            cmd[3] = 0x00;
            cmd[4] = 0x00;
            cmd[5] = 0x40;
            cmd[6] = 0x82;
            _port->send(cmd, 7);
            break;
        case lcExpressScan:
            state = lsProtectionStop; // not implemented
            break;
        case lcGetInfo:
            state = lsProtectionStop; // not implemented
            break;
        case lcGetHealth:
            state = lsProtectionStop; // not implemented
            break;
        case lcGetSampleRate:
            state = lsProtectionStop; // not implemented
            break;
    }
}

void Lidar::sendPoint(Point2D &point, bool first) {
    float a = point.a() * 180 / M_PI;
    while (a < 0) a += 360;
    a = (360 - a) * 64;
    uint16_t ai = roundf(a);

    float d = point.r() * 1000 * 4;
    uint16_t di = roundf(d);

    char data[5];
    data[0] = first ? 0x01 : 0x02;
    data[1] = (ai << 1) + 1;
    data[2] = ai >> 7;
    data[3] = di;
    data[4] = di >> 8;
    sendInBackground(data, 5);
}

void Lidar::sendPoints(std::vector<Point2D> &points) {
    auto data = new uint8_t[points.size()*5];
    size_t p = 0;
    if (state != lsScanning) return;
    std::reverse(points.begin(), points.end());
    for (auto &point: points) {
        float a = point.a() * 180 / M_PI;
        while (a < 0) a += 360;
        a = (360 - a) * 64;
        uint16_t ai = roundf(a);

        float d = point.r() * 1000 * 4;
        uint16_t di = roundf(d);

        data[p+0] = p == 0 ? 0x01 : 0x02;
        data[p+1] = (ai << 1) + 1;
        data[p+2] = ai >> 7;
        data[p+3] = di;
        data[p+4] = di >> 8;

        p += 5;
    }
    sendInBackground(data, p);
}

void Lidar::sendInBackground(void *data, size_t length) {
    mutex.lock();
    dataToSend = data;
    dataLength = length;
    mutex.unlock();
}

void Lidar::sendInBackgroundThreadFunc() {
    while (threadsRun) {
        if (dataToSend && dataLength) {
            mutex.lock();
            _port->send(dataToSend, dataLength);
            free(dataToSend);
            dataToSend = nullptr;
            dataLength = 0;
            mutex.unlock();
        }
    }
}
