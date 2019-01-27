//
// Created by UnSeen on 30/12/2018.
//

#ifndef HEXAPODFIRMWARE_LIDARAPI_H
#define HEXAPODFIRMWARE_LIDARAPI_H

#include <vector>
#include "Serial.h"

typedef enum {
    lcStop = 0x25,
    lcReset = 0x40,
    lcScan = 0x20,
    lcExpressScan = 0x82,
    lcForceScan = 0x21,
    lcGetInfo = 0x50,
    lcGetHealth = 0x52,
    lcGetSampleRate = 0x59
} LidarCommand;

struct LidarSample {
    float angle;
    float distance;
    uint8_t quality;
};

class LidarAPI {

    public:
        explicit LidarAPI(Serial *port) {
            _port = port;
        }

        void sendCommand(LidarCommand command);
        void waitForScanStart();
        bool scanning() const { return _scanning; }
        LidarSample readSample();
        std::vector<LidarSample> readFullCircle();

    private:
        Serial *_port;
        bool _scanning = false;
        LidarSample prevFirst {};
};


#endif //HEXAPODFIRMWARE_LIDARAPI_H
