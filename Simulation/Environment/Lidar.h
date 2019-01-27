//
// Created by UnSeen on 30/12/2018.
//

#ifndef LIDARHEXAPOD_LIDAR_H
#define LIDARHEXAPOD_LIDAR_H

#include <vector>
#include <mutex>
#include "FirmwareCode/Serial.h"
#include "Point2D.h"

typedef enum {
    lsIdle,
    lsRequestProcessing,
    lsScanning,
    lsProtectionStop
} LidarState;

typedef enum {
    lcNone = 0,
    lcStop = 0x25,
    lcReset = 0x40,
    lcScan = 0x20,
    lcExpressScan = 0x82,
    lcForceScan = 0x21,
    lcGetInfo = 0x50,
    lcGetHealth = 0x52,
    lcGetSampleRate = 0x59
} LidarCommand;

class Lidar {

    public:
        explicit Lidar(Serial *port);

        double refreshRate = -5;
        double maxDistance = 12;
        double angleNoise = 0.2;
        double distanceNoise = 0.01;
        int pointsPerRevolution = 200;

        void sendPoint(Point2D &point, bool first);
        void sendPoints(std::vector<Point2D> &points);

    private:
        Serial *_port;
        LidarState state = lsIdle;

        void serialReadThreadFunc();
        bool threadsRun;

        void gotCommand(LidarCommand command);

        void *dataToSend = nullptr;
        size_t dataLength = 0;
        void sendInBackground(void *data, size_t length);
        void sendInBackgroundThreadFunc();

        std::mutex mutex;

};


#endif //LIDARHEXAPOD_LIDAR_H
