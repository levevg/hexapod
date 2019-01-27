#include "Hexapod.h"
#include <iostream>
#include <thread>
#include "AbstractBehavior.h"

#ifdef _WIN32
#include "windows.h"
#else
#include <unistd.h>
#endif // _WIN32

void Hexapod::setGeometry(Geometry g)
{
    _geometry = g;
}

Joint *Hexapod::initJoint(uint8_t servoId, float minServoDelay, float maxServoDelay)
{
    joints[servoId] = new Joint(servoId, minServoDelay, maxServoDelay);
    return joints[servoId];
}

void Hexapod::initLimb(LimbIndex index, Joint *wrist, Joint *knee, Joint *shoulder, float shoulderDirection)
{
    limbs[index] = new Limb(wrist, knee, shoulder, shoulderDirection, this);
    limbs[index]->index = index;
}

void Hexapod::setBehavior(AbstractBehavior *behavior) {
    if (_behavior) {
        _behavior->die();
    }
    _behavior = behavior;
}

bool Hexapod::live() {
    if (!_behavior) return false;
    return _behavior->live();
}
/*
void Hexapod::moveAllToStanceDelta(float dx, float dy, float dz, uint16_t time)
{
    for (auto &limb : limbs)
		limb->moveToStanceDelta(dx, dy, dz, time);
}

void Hexapod::moveOddToStanceDelta(float dx, float dy, float dz, uint16_t time)
{
    for (uint8_t _i = 1; _i < liTotal; _i+=2)
        limbs[_i]->moveToStanceDelta(dx, dy, dz, time);
}

void Hexapod::moveEvenToStanceDelta(float dx, float dy, float dz, uint16_t time)
{
    for (uint8_t _i = 0; _i < liTotal; _i+=2)
        limbs[_i]->moveToStanceDelta(dx, dy, dz, time);
}

inline float seqValue(SequencePoint *seq, uint16_t time) {
	uint16_t k = 0;
	while (seq[k].time <= time) ++k;
	return seq[k-1].value + (seq[k].value - seq[k-1].value) * (time - seq[k-1].time + 1) / (seq[k].time - seq[k-1].time);
}
*/