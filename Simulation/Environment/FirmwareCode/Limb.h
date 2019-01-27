#ifndef LIMB_H
#define LIMB_H

#include "Joint.h"
#include "Hexapod.h"

class Hexapod;

class Limb
{
    public:
        Limb(Joint *wristJoint, Joint *kneeJoint, Joint *shoulderJoint, float shoulderDirection, Hexapod *hexapod);

        Joint *wrist() { return _wrist; };
        Joint *knee() { return _knee; };
        Joint *shoulder() { return _shoulder; };
        Hexapod *hexapod() { return _hexapod; };

        void normalizedToCoords(float kneeNormalized, float wristNormalized, float shoulderNormalized, float &x, float &y, float &z);
        void coorsToNormalized(float x, float y, float z, float &kneeNormalized, float &wristNormalized, float &shoulderNormalized);

        float deltaX() { return currentCoordsX - stanceX; };
        float deltaY() { return currentCoordsY - stanceY; };
        float deltaZ() { return currentCoordsZ - stanceZ; };

        void setYawOffset(float offset) { _yawOffset = offset; }

        bool enabled;

        Hexapod *_hexapod;
        Joint *_wrist, *_knee, *_shoulder;
        float _shoulderOffsetAngle;
        float _yawOffset;
        float shoulderOffsetX, shoulderOffsetY;
        float stanceX, stanceY, stanceZ;
        float currentCoordsX, currentCoordsY, currentCoordsZ;
        float prevCoordsX, prevCoordsY, prevCoordsZ;
        uint16_t currentKneeWidth, currentWristWidth, currentShoulderWidth;

        void updateCurrentPositions();
        bool touchingGround = false;

        int index;
};

#endif // LIMB_H
