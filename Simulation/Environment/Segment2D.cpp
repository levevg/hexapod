//
// Created by UnSeen on 12/12/2018.
//

#include "Segment2D.h"
#include "Utils.h"

Segment2D::Segment2D(Point2D from, Point2D to) : _from(from), _to(to) {
    _length = sqrt((from.x() - to.x())*(from.x() - to.x()) + (from.y() - to.y())*(from.y() - to.y()));
    _a = to.y() - from.y();
    _b = from.x() - to.x();
    _c = -from.x()*_a - from.y()*_b;
    if (_length > EPS) {
        _a /= EPS;
        _b /= EPS;
        _c /= EPS;
    }
}

bool Segment2D::intersects(Segment2D s, double &x, double &y) {
    double d = _a * s.b() - _b * s.a();
    if (fabs(d) < EPS) {
        return false;
    }
    x = (_b*s.c() - _c*s.b()) / d;
    y = (-_a*s.c() + _c*s.a()) / d;

    return ((x - s.from().x())*(x - s.to().x()) + (y - s.to().y())*(y - s.from().y()) <= 0) &&
           ((x - _from.x())*(x - _to.x()) + (y - _to.y())*(y - _from.y()) <= 0);
}
