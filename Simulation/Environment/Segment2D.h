//
// Created by UnSeen on 12/12/2018.
//

#ifndef LIDARHEXAPOD_SEGMENT_H
#define LIDARHEXAPOD_SEGMENT_H

#include "Point2D.h"

struct Segment2D {
public:
    Segment2D(Point2D from, Point2D to);

    Point2D from() { return _from; }
    Point2D to() { return _to; }
    double length() { return _length; }
    double a() { return _a; }
    double b() { return _b; }
    double c() { return _c; }

    friend Segment2D operator + (Segment2D lhs, const Point2D& rhs) {
        return {lhs.from() + rhs, lhs.to() + rhs};
    }

    friend Segment2D operator - (Segment2D lhs, const Point2D& rhs) {
        return {lhs.from() - rhs, lhs.to() - rhs};
    }

    bool intersects(Segment2D s, double &x, double &y);

private:
    Point2D _from, _to;
    double _length, _a, _b, _c;
};

#endif //LIDARHEXAPOD_SEGMENT_H
