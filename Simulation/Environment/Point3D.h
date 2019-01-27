//
// Created by UnSeen on 11/12/2018.
//

#ifndef LIDARHEXAPOD_POINT3D_H
#define LIDARHEXAPOD_POINT3D_H

#include <GL/gl.h>

struct Point3D {
    public:
        Point3D(double x, double y, double z): _x(x), _y(y), _z(z) {}

        double x() const { return _x; }
        double y() const { return _y; }
        double z() const { return _z; }

        Point3D operator += (const Point3D& rhs) {
            _x += rhs.x();
            _y += rhs.y();
            _z += rhs.z();
            return *this;
        }

        friend Point3D operator + (const Point3D& lhs, const Point3D& rhs) {
            return {lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
        }

        void glVertex() { glVertex3d(_x, _y, _z); };
    protected:
        Point3D() = default;;
        double _x, _y, _z;
};

#endif //LIDARHEXAPOD_POINT3D_H
