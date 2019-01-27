//
// Created by UnSeen on 28-Jan-18.
//

#include "FPSCounter.h"

void FPSCounter::tick() {
    ++frames;
    int time = glutGet(GLUT_ELAPSED_TIME);
    frameInterval = time - lastFrameTime;
    lastFrameTime = time;
    if (time - sinceTime >= updateInterval) {
        fps = ((double)updateInterval * frames) / (time - sinceTime);
        sinceTime = time;
        frames = 0;
    }
}
