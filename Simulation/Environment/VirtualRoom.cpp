//
// Created by UnSeen on 12/12/2018.
//

#include <GL/gl.h>
#include "VirtualRoom.h"
#include "Utils.h"

VirtualRoom::VirtualRoom() {
    Point2D corners[] = {Point2D(-3, -2.5, false),
                         Point2D(3, -2.5, false),
                         Point2D(3.5, 2.5, false),
                         Point2D(-3, 2.7, false),
                         Point2D(-3, 0.2, false),
                         Point2D(-2, 0.2, false),
                         Point2D(-2, -0.2, false),
                         Point2D(-3, -0.2, false)
                         };
    int totalCorners = sizeof(corners) / sizeof(corners[0]);
    for (int i = 0; i < totalCorners; ++i) {
        walls.emplace_back(corners[i], corners[(i+1)%totalCorners]);
    }

    walls.emplace_back(Point2D(0, 0, false), Point2D(0.3, 0, false));
    walls.emplace_back(Point2D(0.3, 0, false), Point2D(0.3, 0.3, false));
    walls.emplace_back(Point2D(0.3, 0.3, false), Point2D(0, 0.3, false));
    walls.emplace_back(Point2D(0, 0.3, false), Point2D(0, 0, false));

    placeCircleObstacle(Point2D(1.8, -1, false), 0.4);

    placeCircleObstacle(Point2D(-1.8, 2, false), 0.04);
    placeCircleObstacle(Point2D(-1.8, 1.6, false), 0.04);
    placeCircleObstacle(Point2D(-1.4, 2, false), 0.04);
    placeCircleObstacle(Point2D(-1.4, 1.6, false), 0.04);
}

void VirtualRoom::placeRectObstacle(Point2D center, double width, double height, double angle) {

}

void VirtualRoom::placeCircleObstacle(Point2D center, double radius) {
    double aStep = M_PI/10;
    for (double a = 0; a < 2*M_PI; a += aStep) {
        walls.emplace_back(Point2D(center.x() + cos(a)*radius, center.y() + sin(a)*radius, false),
                           Point2D(center.x() + cos(a + aStep)*radius, center.y() + sin(a+aStep)*radius, false));
    }
}

bool VirtualRoom::findOcclusionForRay(Segment2D ray, double &nx, double &ny, double &dist) {
    double nearest = 1e10;
    for (auto& wall: walls) {
        if ((ray.a() * wall.from().x() + ray.b() * wall.from().y() + ray.c()) * (ray.a() * wall.to().x() + ray.b() * wall.to().y() + ray.c()) > 0) continue;
        double x, y;
        if (ray.intersects(wall, x, y)) {
            double dd = (x - ray.from().x())*(x - ray.from().x()) + (y - ray.from().y())*(y - ray.from().y());
            if (dd < nearest) {
                nearest = dd;
                nx = x;
                ny = y;
            }
        }
    }
    dist = sqrt(nearest);
    return dist < ray.length();
}

Segment2D VirtualRoom::occludedRay(Segment2D ray) {
    double nx, ny, d;
    if (findOcclusionForRay(ray, nx, ny, d)) {
        return {ray.from(), Point2D(nx, ny, false)};
    } else {
        return ray;
    }
}

void VirtualRoom::displayWalls() {
    glBegin(GL_LINES);
    for (auto& wall: walls) {
        glVertex3d(wall.from().x(), wall.from().y(), 0);
        glVertex3d(wall.to().x(), wall.to().y(), 0);
    }
    glEnd();
}
