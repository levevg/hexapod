//
// Created by UnSeen on 13/12/2018.
//

#include <GL/gl.h>
#include <algorithm>
#include "Pod.h"
#include "Utils.h"
#include "FirmwareCode/Config.h"

Pod::Pod(Point2D location, double orientation, VirtualRoom *room) :
        _location(location), _orientation(orientation), _room(room) {
    resetTimers();
    _prevTick = 0;
    _lidar = new Lidar(&lidarPort);
}

void Pod::resetTimers() {
    _prevTick = currentTime();
    _latestPointsTime = -1;
    _latestPointsLocation = _location;
}

void Pod::display(bool follow, bool mode3d) {
    if (!follow) {
        glTranslated(_location.x(), _location.y(), 0);
        glRotated(_orientation*180/M_PI, 0, 0, 1);
    }

    if (mode3d) {
        drawPod3D();
    } else {
        drawPod2D();
    }

    int time = glutGet(GLUT_ELAPSED_TIME);
    double rotations = _lidar->refreshRate * time * timewarp / 1000;
    double rayAngle = (rotations - floor(rotations)) * M_PI * 2;
    Segment2D ray(Point2D(rayAngle, visualRadius, true), Point2D(rayAngle, _lidar->maxDistance, true));
    ray = _room->occludedRay(ray + _location) - _location;

    glColor4f(1, 0, 0, 0.2);
    glBegin(GL_LINE_LOOP);
    Point2D(ray.from().a() - _orientation, ray.from().r(), true).glVertex();
    Point2D(ray.to().a() - _orientation, ray.to().r(), true).glVertex();
    glEnd();

    glColor4f(0, 1, 0, 1);
    glBegin(GL_POINTS);
    for (auto &point: _latestPoints) {
        point.glVertex();
    }
    glEnd();

    glRotated(-_orientation*180/M_PI, 0, 0, 1);
    glTranslated(-_location.x(), -_location.y(), 0);
}

void Pod::drawPod2D() {
    glColor4f(1, 1, 1, 1);
    glBegin(GL_LINE_LOOP);
    for (double a = M_PI/6; a < 6; a += M_PI/3) {
        Point2D(a, visualRadius, true).glVertex();
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3d(0.5 * visualRadius, 0, 0);
    glVertex3d(1 * visualRadius, 0, 0);
    glEnd();
}

void Pod::drawPod3D() {
    double botPlateZ = _elevation, topPlateZ = botPlateZ + 0.06,
           topPlateRadius = hexapod.geometry()->centerToShoulder;

    glDepthFunc(GL_LESS);

    glColor4f(1, 1, 1, 1);
    glBegin(GL_LINE_LOOP);
    for (double a = M_PI/6; a < 6; a += M_PI/3) {
        Point2D(a, topPlateRadius, true).raise(botPlateZ).glVertex();
    }
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (double a = M_PI/6; a < 6; a += M_PI/3) {
        Point2D(a, topPlateRadius, true).raise(topPlateZ).glVertex();
    }
    glEnd();
    glBegin(GL_LINES);
    for (double a = M_PI/6; a < 6; a += M_PI/3) {
        Point2D(a, topPlateRadius, true).raise(botPlateZ).glVertex();
        Point2D(a, topPlateRadius, true).raise(topPlateZ).glVertex();
    }
    glEnd();

    glBegin(GL_LINES);
    glVertex3d(0.5 * visualRadius, 0, topPlateZ);
    glVertex3d(1 * visualRadius, 0, topPlateZ);
    glEnd();

    glColor4f(0, 0, 0, 0.8);
    glBegin(GL_TRIANGLE_FAN);
    Point3D(0, 0, 0).glVertex();
    for (double a = M_PI/6; a < 7; a += M_PI/3) {
        Point2D(a, topPlateRadius - 0.005, true).raise(topPlateZ).glVertex();
    }
    glEnd();
    for (double a = M_PI/6; a < 7; a += M_PI/3) {
        glBegin(GL_TRIANGLE_STRIP);
        Point2D(a, topPlateRadius - 0.005, true).raise(botPlateZ + 0.005).glVertex();
        Point2D(a, topPlateRadius - 0.005, true).raise(topPlateZ - 0.005).glVertex();
        Point2D(a + M_PI/3, topPlateRadius - 0.005, true).raise(botPlateZ + 0.005).glVertex();
        Point2D(a + M_PI/3, topPlateRadius - 0.005, true).raise(topPlateZ - 0.005).glVertex();
        glEnd();
    }

    glLineWidth(1);
    glPointSize(3);

    const Geometry *g = hexapod.geometry();
    for (auto& limb: hexapod.limbs) {
        float kneeNormalized = limb->knee()->currentPosition,
              wristNormalized = limb->wrist()->currentPosition,
              shoulderNormalized = limb->shoulder()->currentPosition;
        float k = g->kneeAngleSpan() * kneeNormalized + g->kneeMinAngle;
        float w = g->wristAngleSpan() * wristNormalized + g->wristMinAngle;
        float s = g->shoulderAngleSpan() * shoulderNormalized + g->shoulderMinAngle;

        double sa = limb->_shoulderOffsetAngle - M_PI/2, ta = sa + s + hexapod.yaw;
        Point3D shoulder = Point2D(sa, topPlateRadius, true).raise(botPlateZ + hexapod.geometry()->kneeToBottom);
        Point3D knee = shoulder + Point2D(ta, hexapod.geometry()->kneeToShoulder, true).raise(0);
        Point3D wrist = knee + Point2D(ta, hexapod.geometry()->wristToKnee * sinf(k), true).raise(hexapod.geometry()->wristToKnee * cosf(k));
        Point3D tip = wrist + Point2D(ta, hexapod.geometry()->tipToWrist * sinf(k + w), true).raise(hexapod.geometry()->tipToWrist * cosf(k + w));

        glColor4f(1, 1, 1, 1);
        glBegin(GL_LINE_STRIP);
        shoulder.glVertex();
        knee.glVertex();
        wrist.glVertex();
        tip.glVertex();
        glEnd();

        glBegin(GL_POINTS);
        shoulder.glVertex();
        knee.glVertex();
        wrist.glVertex();
        if (limb->touchingGround) {
            glColor4f(1, 0, 0, 1);
            tip.glVertex();
        }
        glEnd();
    }

    glPointSize(1);
    glLineWidth(1);
}

double Pod::currentTime() {
    return Utils::time()*timewarp;
}

void Pod::updateJointsPositions() {
    for (auto &limb: hexapod.limbs) {
        float realTime = Utils::time();
        limb->wrist()->updateCurrentPosition(realTime);
        limb->knee()->updateCurrentPosition(realTime);
        limb->shoulder()->updateCurrentPosition(realTime);
    }
}

void Pod::tick() {
    updateJointsPositions();

    std::vector<Limb *> ordered, movers;
    for (auto &limb: hexapod.limbs) {
        limb->updateCurrentPositions();
        ordered.emplace_back(limb);
    }
    std::sort(ordered.begin(), ordered.end(),
        [](const Limb *a, const Limb *b) -> bool {
            return a->currentCoordsZ < b->currentCoordsZ;
        });
    _elevation = 0;
    if (!ordered.empty()) _elevation = fmaxf(_elevation, -ordered[0]->currentCoordsZ);
    for (int i = 0; i < ordered.size(); ++i) {
        bool touching = (i < 3) || (ordered[i]->currentCoordsZ - ordered[0]->currentCoordsZ < 0.001);
        if (ordered[i]->touchingGround && touching) {
            movers.emplace_back(ordered[i]);
        }
        ordered[i]->touchingGround = touching;
    }

    if (movers.size() > 1) {
        float px = 0, py = 0, cx = 0, cy = 0, ms = movers.size(), da = 0;
        for (auto &limb: movers) {
            px += limb->prevCoordsX;
            py += limb->prevCoordsY;
            cx += limb->currentCoordsX;
            cy += limb->currentCoordsY;
        }
        px /= ms;
        cx /= ms;
        py /= ms;
        cy /= ms;
        Point2D delta(px - cx, py - cy, false);
        for (auto &limb: movers) {
            float a = Point2D(limb->prevCoordsX - px, limb->prevCoordsY - py, false).a() - Point2D(limb->currentCoordsX - cx, limb->currentCoordsY - cy, false).a();
            while (fabsf(a - 2*M_PI) < fabsf(a)) a -= 2*M_PI;
            while (fabsf(a + 2*M_PI) < fabsf(a)) a += 2*M_PI;
            da += a;
        }
        da /= ms;
        _location += Point2D(delta.a() - M_PI/2 + _orientation, delta.r(), true);
        _orientation += da;
    }

    double time = currentTime(), delta = time - _prevTick;
    _prevTick = time;


    if (!pointsProcessed) {
        processPoints();
    }

    if (time - _latestPointsTime > 1/fabsf(_lidar->refreshRate)) {
        updatePoints();
    }
}

void Pod::processPoints() {
    _lidar->sendPoints(_latestPoints);
    pointsProcessed = true;
}

void Pod::updatePoints() {
    std::vector<Point2D> points;
    double phiStep = 2*M_PI/_lidar->pointsPerRevolution;
    Point2D l = _latestPointsLocation, d = (_location - l) / _lidar->pointsPerRevolution;
    for (double a = _orientation; a < _orientation + 2*M_PI; a += phiStep) {
        double phi = a + Utils::frand()*_lidar->angleNoise*phiStep;
        Segment2D ray(l, l + Point2D(phi, _lidar->maxDistance, true));
        double nx, ny, dist;
        if (_room->findOcclusionForRay(ray, nx, ny, dist)) {
            d *= 1 + Utils::frand() * _lidar->distanceNoise;
            phi = a + Utils::frand()*_lidar->angleNoise*phiStep - _orientation;
            points.emplace_back(ray.from() + Point2D(phi, dist, true) - l);
        }
        l += d;
    }
    _latestPoints = points;
    _latestPointsTime = currentTime();
    _latestPointsLocation = _location;
    pointsProcessed = false;
}

std::string Pod::debugString() {
    if (fabs(timewarp - 1) < EPS) return "";
    char time[20];
    if (timewarp - floor(timewarp) < EPS) {
        sprintf(time, "Time x%.0f", timewarp);
    } else {
        sprintf(time, "Time x%.3f", timewarp);
    }
    return std::string(time);
}

void Pod::setTimewarp(double t) {
    timewarp = t;
    resetTimers();
}

void Pod::setLocation(Point2D newLocation) {
    _location = newLocation;
    _latestPointsTime = 0;
}

void Pod::lookAt(Point2D point) {
    _orientation = (point - _location).a();
}
