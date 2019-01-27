//
// Created by UnSeen on 30/12/2018.
//

#ifndef HEXAPODFIRMWARE_LIDARBEHAVIOR_H
#define HEXAPODFIRMWARE_LIDARBEHAVIOR_H
#include "Hexapod.h"
#include "Behavior.h"
#include "LidarAPI.h"
#include <mutex>

typedef enum {
    lbRaise,
    lbIdle,
    lbGoForward,
    lbGoForwardAdjustLeft,
    lbGoForwardAdjustRight,
    lbGoLeft,
    lbGoBack,
    lbGoRight,
    lbStopThenNext,
} LidarBehaviorState;

class LidarBehavior : public Behavior<LidarBehaviorState> {
public:
    explicit LidarBehavior(Hexapod *hexapod, LidarAPI *lidar) : Behavior(hexapod) {
        _lidar = lidar;
    };

    void born() override;
    void die() override;
    void fsm(uint32_t stateTime) override;

private:
    uint32_t sideTime = 5000;
    LidarAPI *_lidar;

    void serialReadThreadFunc();

    void processView();
    std::vector<LidarSample> view;
    bool viewProcessed = true;
    std::mutex mutex;

    LidarBehaviorState nextState;

    bool isPathClear(float fx, float fy, float tx, float ty, float cl);

};

#endif //HEXAPODFIRMWARE_LIDARBEHAVIOR_H
