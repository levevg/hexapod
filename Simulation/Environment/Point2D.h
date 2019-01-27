//
// Created by UnSeen on 12/12/2018.
//

#ifndef LIDARHEXAPOD_POINT2D_H
#define LIDARHEXAPOD_POINT2D_H

#include <cmath>
#include <GL/gl.h>
#include <string>
#include "Point3D.h"

struct Point2D {
    public:
        Point2D() = default;;

        Point2D(double xa, double yr, bool polar) {
            if (polar) {
                _a = xa;
                _r = yr;
                _x = _r*cos(_a);
                _y = _r*sin(_a);
            } else {
                _x = xa;
                _y = yr;
                _a = atan2(_y, _x);
                _r = sqrt(_x*_x + _y*_y);
            }
        }

        Point2D operator += (const Point2D& rhs) {
            _x += rhs.x();
            _y += rhs.y();
            _a = atan2(_y, _x);
            _r = sqrt(_x*_x + _y*_y);
            return *this;
        }

        friend Point2D operator + (const Point2D& lhs, const Point2D& rhs) {
            return {lhs.x() + rhs.x(), lhs.y() + rhs.y(), false};
        }

        Point2D operator -= (const Point2D& rhs) {
            _x -= rhs.x();
            _y -= rhs.y();
            _a = atan2(_y, _x);
            _r = sqrt(_x*_x + _y*_y);
            return *this;
        }

        friend Point2D operator - (const Point2D& lhs, const Point2D& rhs) {
            return {lhs.x() - rhs.x(), lhs.y() - rhs.y(), false};
        }

        Point2D operator *= (double rhs) {
            _x *= rhs;
            _y *= rhs;
            _r *= rhs;
            return *this;
        }

        Point2D operator /= (double rhs) {
            _x /= rhs;
            _y /= rhs;
            _r /= rhs;
            return *this;
        }

        friend Point2D operator * (const Point2D& lhs, const Point2D& rhs) {
            return {lhs.x() * rhs.x(), lhs.y() * rhs.y(), false};
        }

        friend Point2D operator * (const Point2D& lhs, double rhs) {
            return {lhs.x() * rhs, lhs.y() * rhs, false};
        }

        friend Point2D operator / (const Point2D& lhs, double rhs) {
            return {lhs.x() / rhs, lhs.y() / rhs, false};
        }

        double x() const { return _x; }
        double y() const { return _y; }
        double a() const { return _a; }
        double r() const { return _r; }

        void glVertex() const { glVertex3d(_x, _y, 0); };

        Point3D raise(double z);

        std::string description() {
            char s[40];
            sprintf_s(s, 40, "(%.2f, %.2f) [%.2f @ %.1f]", _x, _y, _r, _a*180/M_PI);
            return std::string(s);
        }

    protected:
        double _x, _y, _a, _r;
};

#endif //LIDARHEXAPOD_POINT2D_H
