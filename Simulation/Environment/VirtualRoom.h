//
// Created by UnSeen on 12/12/2018.
//

#ifndef LIDARHEXAPOD_VIRTUALROOM_H
#define LIDARHEXAPOD_VIRTUALROOM_H

#include <vector>
#include "Point2D.h"
#include "Segment2D.h"

class VirtualRoom {
public:
    VirtualRoom();

    void displayWalls();
    Segment2D occludedRay(Segment2D ray);
    bool findOcclusionForRay(Segment2D ray, double &nx, double &ny, double &dist);

private:
    std::vector<Segment2D> walls;

    void placeRectObstacle(Point2D center, double width, double height, double angle);

    void placeCircleObstacle(Point2D center, double radius);

};

#endif //LIDARHEXAPOD_VIRTUALROOM_H
