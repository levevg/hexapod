//
// Created by UnSeen on 28-Jan-18.
//

#include <cstdint>
#include <GL/freeglut.h>
#include <string>
#include <utility>
#include "Utils.h"

std::string Utils::status;

void Utils::displayText(double x, double y, uint32_t argb, std::string text, void *font) {
    glColor4d(((argb >> 16) & 0xFF)/255.0, ((argb >> 8) & 0xFF)/255.0, (argb & 0xFF)/255.0, ((argb >> 24) & 0xFF)/255.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    int width = glutGet(GLUT_WINDOW_WIDTH), height = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, width, 0, height);

    if (y > 0) {
        glRasterPos2d(x, height - y - glutBitmapHeight(font));
    } else {
        glRasterPos2d(x, -y);
    }
    glutBitmapString(font, (const unsigned char *) text.c_str());

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Utils::displayStatus() {
    Utils::displayText(10, -30, 0xFFFFFFFF, status);
}

void Utils::displayFPS(std::string fps) {
    Utils::displayText(5, 5, 0xFFFFFFFF, std::move(fps));
}

void Utils::displayCommandLine(const CommandLine &commandLine) {
    if (!commandLine.isActive()) return;
    Utils::displayText(10, -10, 0xFFFFFFFF, commandLine.displayableString());
}

double Utils::frand() {
    return 0.0001 * (rand()%10000 - 5000);
}
