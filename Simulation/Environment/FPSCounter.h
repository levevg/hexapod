//
// Created by UnSeen on 28-Jan-18.
//

#ifndef LIDARHEXAPOD_FPSCOUNTER_H
#define LIDARHEXAPOD_FPSCOUNTER_H


#include <GL/freeglut.h>
#include <string>

class FPSCounter {
public:
    explicit FPSCounter(int updateInterval = 1000) : updateInterval(updateInterval) { }
    void tick();
    std::string fpsString() { return std::to_string((int)fps); }
    int lastFrameInterval() { return frameInterval; };
private:
    int frames = 0;
    int sinceTime = 0;
    int updateInterval = 1000;
    int frameInterval = 0;
    int lastFrameTime = 0;
    double fps = 0;
};


#endif //LIDARHEXAPOD_FPSCOUNTER_H
