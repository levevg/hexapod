//
// Created by UnSeen on 28-Jan-18.
//

#include <GL/gl.h>
#include <GL/glu.h>
#include "Camera.h"

Camera::Camera() = default;

void Camera::lookFromPointToPoint(Point3D eye, Point3D center, Point3D up) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x(), eye.y(), eye.z(), center.x(), center.y(), center.z(), up.x(), up.y(), up.z());
    glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
}

void Camera::apply() {
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd(matrix);
}

void Camera::moveBy(double x, double y, double z) {
    glPushMatrix();
    glLoadIdentity();
    glTranslated(x, y, z);
    glMultMatrixd(matrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
    glPopMatrix();
}

void Camera::moveForward(double timeDelta) {
    moveBy(0, 0, speed*timeDelta);
}

void Camera::moveBackward(double timeDelta) {
    moveBy(0, 0, -speed*timeDelta);
}

void Camera::moveLeft(double timeDelta) {
    moveBy(speed*timeDelta, 0, 0);
}

void Camera::moveRight(double timeDelta) {
    moveBy(-speed*timeDelta, 0, 0);
}

void Camera::moveUp(double timeDelta) {
    moveBy(0, speed*timeDelta, 0);
}

void Camera::moveDown(double timeDelta) {
    moveBy(0, -speed*timeDelta, 0);
}