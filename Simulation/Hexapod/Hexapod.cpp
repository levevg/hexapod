#include "Hexapod.h"
#include <iostream>
#include <math.h>
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

void Hexapod::raiseSequence()
{
	for (auto &limb : limbs) {
		limb->moveKnee(0.0, 1500);
		limb->moveWrist(0.2, 1500);
	}
	delayMs(1500);
	for (auto &limb : limbs) {
		limb->moveShoulder(0.5, 500);
		limb->moveWrist(1, 500);
	}
	delayMs(500);
	for (auto &limb : limbs) {
		limb->updateCurrentPositions();
	}
	float z = limbs[0]->deltaZ();
	uint8_t steps = 10;
	for (uint8_t j = 0; j < steps; ++j) {
		for (auto &limb : limbs) {
			limb->moveToStanceDelta(limb->deltaX(), limb->deltaY(), z * (steps - j - 1) / steps, 1000 / steps);
		}
		delayMs(1000 / steps);
	}
	delayMs(200);
	for (uint8_t i = 0; i < liTotal; i+=2) {
		limbs[i]->moveToStanceDelta(limbs[i]->deltaX(), limbs[i]->deltaY(), 40, 500);
	}
	delayMs(500);
    moveEvenToStanceDelta(0, 0, 0, 500);
	delayMs(500);
	moveOddToStanceDelta(0, 0, 40, 500);
	delayMs(500);
	moveOddToStanceDelta(0, 0, 0, 500);
	delayMs(500);
}

void Hexapod::battlePoseSequence()
{
	moveAllToStanceDelta(0, 50, 20, 500);
	limbs[liLeftFront]->moveKnee(0, 500);
	limbs[liLeftFront]->moveWrist(0.3, 500);
	limbs[liLeftFront]->moveShoulder(0.4, 500);

	limbs[liRightFront]->moveKnee(0.0, 500);
	limbs[liRightFront]->moveWrist(0.3, 500);
	limbs[liRightFront]->moveShoulder(0.6, 500);

	delayMs(2000);
	moveAllToStanceDelta(0, 0, 0, 500);
	delayMs(500);
}

inline float seqValue(SequencePoint *seq, uint16_t time) {
	uint16_t k = 0;
	while (seq[k].time <= time) ++k;
	return seq[k-1].value + (seq[k].value - seq[k-1].value) * (time - seq[k-1].time + 1) / (seq[k].time - seq[k-1].time);
}

#define SEQDIV 1

void Hexapod::stepSequence(int8_t mx, int8_t my, int8_t a, bool first, bool last)
{
	float h = 40;
	uint16_t delay = 50/SEQDIV; // 200!
	float ap = first ? 0 : -1,
           al = last ?  0 : -1;
	SequencePoint oddXY[6], oddZ[5], evenXY[6], evenZ[5], angO[5];

	oddXY[0].value = ap;    oddXY[0].time = 0*SEQDIV;
	oddXY[1].value = ap;    oddXY[1].time = 1*SEQDIV;
	oddXY[2].value = 1;     oddXY[2].time = 7*SEQDIV;
	oddXY[3].value = 1;     oddXY[3].time = 8*SEQDIV;
	oddXY[4].value = al;    oddXY[4].time = 15*SEQDIV;
	oddXY[5].value = al;    oddXY[5].time = 16*SEQDIV;

	oddZ[0].value = 0; oddZ[0].time = 0*SEQDIV;
	oddZ[1].value = h; oddZ[1].time = 2*SEQDIV;
	oddZ[2].value = h; oddZ[2].time = 6*SEQDIV;
	oddZ[3].value = 0; oddZ[3].time = 8*SEQDIV;
	oddZ[4].value = 0; oddZ[4].time = 16*SEQDIV;

	evenXY[0].value = -ap;    evenXY[0].time = 0*SEQDIV;
	evenXY[1].value = -ap;    evenXY[1].time = 1*SEQDIV;
	evenXY[2].value = -1;     evenXY[2].time = 8*SEQDIV;
	evenXY[3].value = -1;     evenXY[3].time = 9*SEQDIV;
	evenXY[4].value = -al;    evenXY[4].time = 15*SEQDIV;
	evenXY[5].value = -al;    evenXY[5].time = 16*SEQDIV;

	evenZ[0].value = 0; evenZ[0].time = 0*SEQDIV;
	evenZ[1].value = 0; evenZ[1].time = 8*SEQDIV;
	evenZ[2].value = h; evenZ[2].time = 10*SEQDIV;
	evenZ[3].value = h; evenZ[3].time = 14*SEQDIV;
	evenZ[4].value = 0; evenZ[4].time = 16*SEQDIV;

	angO[0].value = 0;     angO[0].time = 0*SEQDIV;
	angO[1].value = 0;     angO[1].time = 2*SEQDIV;
	angO[2].value = a;     angO[2].time = 8*SEQDIV;
	angO[3].value = 0;     angO[3].time = 14*SEQDIV;
	angO[4].value = 0;     angO[4].time = 16*SEQDIV;

	for (uint8_t i = 0; i < 16*SEQDIV; ++i) {
		float a, b;
		a = seqValue(angO, i);
		//std::cout << a << "   (";
		for (uint8_t k = 1; k < 6; k+=2) limbs[k]->setYawOffset(a*M_PI/180);
		a = seqValue(oddXY, i);
		b = seqValue(oddZ, i);
		moveOddToStanceDelta(mx*a, my*a, b, delay);
		a = seqValue(evenXY, i);
		b = seqValue(evenZ, i);
		moveEvenToStanceDelta(mx*a, my*a, b, delay);
		//std::cout << mx*a << " " << my*a << " " << b << ")   " << delay << "\n";
		delayMs(delay);
	}

// TIME   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
//  L     C   C   -   -   -   -   -   x   x   -   -   -   -   -   -  -x  -x
//  H     0   -   h   -   -   -   h   -   0   -   -   -   -   -   -   -   0

//  L     C   C   -   -   -   -   -   -  -x  -x   -   -   -   -   -   x   x
//  H     0   -   -   -   -   -   -   -   0   -   h   -   -   -   h   -   0
//  AO    0       0                       a                       0       0
//  AO

}
