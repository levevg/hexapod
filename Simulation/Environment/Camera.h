//
// Created by UnSeen on 28-Jan-18.
//

#ifndef LIDARHEXAPOD_CAMERA_H
#define LIDARHEXAPOD_CAMERA_H

#include "Point3D.h"

class Camera {
public:
    Camera();

    void apply();

    void lookFromPointToPoint(Point3D eye, Point3D center, Point3D up);

    void moveForward(double timeDelta);
    void moveBackward(double timeDelta);
    void moveLeft(double timeDelta);
    void moveRight(double timeDelta);
    void moveBy(double x, double y, double z);
    void moveUp(double timeDelta);
    void moveDown(double timeDelta);

    double speed = 0.02;

private:
    GLdouble matrix[16];

};

#endif //LIDARHEXAPOD_CAMERA_H
