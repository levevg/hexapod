//
// Created by UnSeen on 28-Jan-18.
//

#ifndef LIDARHEXAPOD_UTILS_H
#define LIDARHEXAPOD_UTILS_H

#define EPS 1e-10

#include <string>
#include <utility>
#include <GL/freeglut.h>
#include "CommandLine.h"

class Utils {
public:
    static void displayText(double x, double y, uint32_t argb, std::string text, void *font = GLUT_BITMAP_9_BY_15);
    static void displayStatus();
    static void displayFPS(std::string fps);
    static void displayCommandLine(const CommandLine &commandLine);
    static void setStatus(std::string newStatus) { Utils::status = std::move(newStatus); }

    static double frand();

    static double time() { return 0.001 * glutGet(GLUT_ELAPSED_TIME); }
private:
    static std::string status;
};


#endif //LIDARHEXAPOD_UTILS_H
