#ifndef HEXAPOD_H
#define HEXAPOD_H

#include "Joint.h"
#include "Limb.h"
#include <cstdint>

void delayMs(uint16_t delay);

class Limb;
class AbstractBehavior;

typedef enum {
    liLeftFront = 0,
    liLeftMiddle,
    liLeftRear,
    liRightFront,
    liRightMiddle,
    liRightRear,

    liTotal
} LimbIndex;

typedef struct {
    float tipToWrist, wristToKnee, kneeToShoulder, centerToShoulder, kneeToBottom, kneeMinAngle,
          kneeMaxAngle, wristMinAngle, wristMaxAngle, shoulderMinAngle, shoulderMaxAngle,
          stanceKneeNormalized, stanceWristNormalized, stanceShoulderNormalized;
    float kneeAngleSpan() const { return kneeMaxAngle - kneeMinAngle; };
    float wristAngleSpan() const { return wristMaxAngle - wristMinAngle; };
    float shoulderAngleSpan() const { return shoulderMaxAngle - shoulderMinAngle; };
} Geometry;


struct SequencePoint;
struct SequencePoint {
	float value;
	uint16_t time;
};

class Hexapod
{
    public:
        Hexapod() { _behavior = nullptr; }

        void setBehavior(AbstractBehavior *behavior);
        void setGeometry(Geometry g);
        const Geometry *geometry() const { return &_geometry; };
        Joint *initJoint(uint8_t servoId, float minServoDelay, float maxServoDelay);
        void initLimb(LimbIndex index, Joint *wrist, Joint *knee, Joint *shoulder, float shoulderDirection);
        Limb *limb(LimbIndex index) { return limbs[index]; };
        float yaw = 0;

        void moveAllToStanceDelta(float dx, float dy, float dz, uint16_t time);
        void moveOddToStanceDelta(float dx, float dy, float dz, uint16_t time);
        void moveEvenToStanceDelta(float dx, float dy, float dz, uint16_t time);

        void raiseSequence();
        void battlePoseSequence();
        void stepSequence(int8_t mx, int8_t my, int8_t a, bool first, bool last);

        bool live();

    private:
        Geometry _geometry {};
        Joint *joints[32]{};
        Limb *limbs[6]{};

        AbstractBehavior *_behavior;
};

#endif // HEXAPOD_H
