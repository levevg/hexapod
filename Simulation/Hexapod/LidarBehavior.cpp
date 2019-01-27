//
// Created by UnSeen on 30/12/2018.
//

#include <thread>
#include <iostream>
#include <math.h>
#include "LidarBehavior.h"

void LidarBehavior::born() {
    _alive = true;
    _lidar->sendCommand(lcScan);
    //setState(lbRaise);
    setState(lbIdle);

    std::thread readThread(&LidarBehavior::serialReadThreadFunc, this);
    readThread.detach();
}

void LidarBehavior::die() {
    _lidar->sendCommand(lcStop);
    _alive = false;
}

void LidarBehavior::fsm(uint32_t stateTime) {
    processView();
    switch (_state) {
        case lbRaise:
            _hexapod->raiseSequence();
            delayMs(500);
            setState(lbIdle);
            break;
        case lbIdle:
            break;
        case lbGoForward:
            moveY = 30;
            break;
        case lbGoForwardAdjustLeft:
            moveY = 30;
            moveA = -20;
            setState(lbGoForward);
            break;
        case lbGoForwardAdjustRight:
            moveY = 30;
            moveA = 20;
            setState(lbGoForward);
            break;
        case lbGoLeft:
            moveX = -30;
            break;
        case lbGoBack:
            moveY = -30;
            break;
        case lbGoRight:
            moveX = 30;
            break;
        case lbStopThenNext:
            setState(nextState);
            break;
    }
}

void LidarBehavior::serialReadThreadFunc() {
    while (_alive) {
        if (_lidar->scanning()) {
            auto circle = _lidar->readFullCircle();
            mutex.lock();
            view = circle;
            viewProcessed = view.empty();
            mutex.unlock();
        } else {
            _lidar->waitForScanStart();
        }
    }
}

bool LidarBehavior::isPathClear(float fx, float fy, float tx, float ty, float cl) {
    cl = cl * cl;
    for (auto sample: view) {
        float sx = sample.distance * cos(sample.angle),
              sy = sample.distance * sin(sample.angle);
//        bool inStartCircle = (sx - fx)*(sx - fx) + (sy - fy)*(sy - fy) < cl;
        bool inFinishCircle = (sx - tx)*(sx - tx) + (sy - ty)*(sy - ty) < cl;
        if (inFinishCircle && (tx - fx) * sx + (ty - fy) * sy > 0) return false;
    }
    return true;
}

void LidarBehavior::processView() {
    mutex.lock();
    if (viewProcessed) {
        mutex.unlock();
        return;
    }

    float minDistance = 1e10, mda = 1e10;
    for (auto sample: view) {
        if (sample.distance < minDistance) {
            minDistance = sample.distance;
            mda = sample.angle;
        }
    }
    std::cout << "Min distance: " << minDistance << "mm @" << mda << "\n";

    struct {
        LidarBehaviorState state;
        int dx, dy;
    } possibleStates[] = {
        {lbGoForward, 0, 600},
        {lbGoForwardAdjustLeft, -140, 300},
        {lbGoForwardAdjustRight, 140, 300},
        {lbGoForward, 0, 150},
        {lbGoRight, 150, 0},
        {lbGoLeft, -150, 0},
        {lbGoBack, -150, 0},
    };

    nextState = lbIdle;
    for (auto s: possibleStates) {
        if (isPathClear(0, 0, s.dx, s.dy, 300)) {
            nextState = s.state;
            break;
        }
    }
    if (nextState != lbIdle) {
        setState(lbStopThenNext);
    } else {
        setState(lbIdle);
    }

    viewProcessed = true;
    mutex.unlock();
}
