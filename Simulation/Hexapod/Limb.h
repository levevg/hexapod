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

        void moveKnee(float normalized, uint16_t time);
        void moveWrist(float normalized, uint16_t time);
        void moveShoulder(float normalized, uint16_t time);
        void moveToStanceDelta(float dx, float dy, float dz, uint16_t time);
        void moveToPWMWidth(uint16_t kneeWidth, uint16_t wristWidth, uint16_t shoulderWidth, uint16_t time);

        void updateCurrentPositions();

        float deltaX() { return currentCoordsX - stanceX; };
        float deltaY() { return currentCoordsY - stanceY; };
        float deltaZ() { return currentCoordsZ - stanceZ; };

        void setYawOffset(float offset) { _yawOffset = offset; }

        bool enabled;

    private:
        Hexapod *_hexapod;
        Joint *_wrist, *_knee, *_shoulder;
        float _shoulderOffsetAngle;
        float _yawOffset = 0;
        float shoulderOffsetX, shoulderOffsetY;
        float stanceX, stanceY, stanceZ;
        float currentKneeNormalized, currentWristNormalized, currentShoulderNormalized;
        float currentCoordsX, currentCoordsY, currentCoordsZ;
        uint16_t currentKneeWidth, currentWristWidth, currentShoulderWidth;

};

#endif // LIMB_H
