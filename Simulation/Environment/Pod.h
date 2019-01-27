//
// Created by UnSeen on 13/12/2018.
//

#ifndef LIDARHEXAPOD_POD_H
#define LIDARHEXAPOD_POD_H

#include <string>
#include "Point2D.h"
#include "VirtualRoom.h"
#include "Lidar.h"

//typedef enum {
//    psIdle,
//    psGoForward,
//    psGoBackward,
//    psRotateLeft,
//    psRotateRight
//} PodState;

class Pod {
public:
    explicit Pod(Point2D location, double orientation, VirtualRoom *room);

    void display(bool follow, bool mode3d);
    void tick();

    Point2D location() { return _location; }
    double orientation() { return _orientation; }
    void setLocation(Point2D newLocation);
    void lookAt(Point2D point);
    VirtualRoom *room() { return _room; }

    void speedTimeUp() { if (timewarp > 8) return;  setTimewarp(2*timewarp); }
    void speedTimeDown() {  if (timewarp < 0.1) return; setTimewarp(0.5*timewarp); }
    void speedTimeReset() { setTimewarp(1); }

    std::string debugString();

private:
    Lidar *_lidar;

    VirtualRoom *_room;
    Point2D _location;
    double _orientation;
    double _elevation;

    double _prevTick;

    std::vector<Point2D> _latestPoints;
    double _latestPointsTime;
    Point2D _latestPointsLocation;
    bool pointsProcessed;

    double visualRadius = 0.14;

    double timewarp = 1;

    void updatePoints();

    double currentTime();

    void setTimewarp(double t);

    void resetTimers();

    void processPoints();

    void drawPod2D();
    void drawPod3D();

    void updateJointsPositions();
};

#endif //LIDARHEXAPOD_POD_H
