#include "Limb.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include "Serial.h"
#include <GL/gl.h>

Limb::Limb(Joint *wristJoint, Joint *kneeJoint, Joint *shoulderJoint, float shoulderDirection, Hexapod *hexapod)
{
    _hexapod = hexapod;
    _wrist = wristJoint;
    _knee = kneeJoint;
    _shoulder = shoulderJoint;
    _shoulderOffsetAngle = shoulderDirection;

	enabled = true;
	shoulderOffsetX = hexapod->geometry()->centerToShoulder * cosf(_shoulderOffsetAngle);
	shoulderOffsetY = hexapod->geometry()->centerToShoulder * sinf(_shoulderOffsetAngle);

    normalizedToCoords(hexapod->geometry()->stanceKneeNormalized,
                       hexapod->geometry()->stanceWristNormalized,
                       hexapod->geometry()->stanceShoulderNormalized,
                       stanceX, stanceY, stanceZ);
}

float aCosSaturated(float a)
{
	return (a > 1) ? 0 : ((a < -1) ? M_PI : acos(a));
}

void Limb::normalizedToCoords(float kneeNormalized, float wristNormalized, float shoulderNormalized, float &x, float &y, float &z)
{
    const Geometry *g = _hexapod->geometry();
    float k = g->kneeAngleSpan() * kneeNormalized + g->kneeMinAngle;
    float w = g->wristAngleSpan() * wristNormalized + g->wristMinAngle;
    float s = g->shoulderAngleSpan() * shoulderNormalized + g->shoulderMinAngle;

    float sinK = sinf(k), cosK = cosf(k), sinKW = sinf(k + w), cosKW = cosf(k + w),
          sinOS = sinf(_shoulderOffsetAngle + s + _hexapod->yaw + _yawOffset),
          cosOS = cosf(_shoulderOffsetAngle + s + _hexapod->yaw + _yawOffset);

    float d = g->kneeToShoulder + g->wristToKnee * sinK + g->tipToWrist * sinKW;
    x = shoulderOffsetX + d * cosOS;
    y = shoulderOffsetY + d * sinOS;
    z = g->kneeToBottom + g->wristToKnee * cosK + g->tipToWrist * cosKW;
}

void Limb::coorsToNormalized(float x, float y, float z, float &kneeNormalized, float &wristNormalized, float &shoulderNormalized)
{
    const Geometry *g = _hexapod->geometry();

	float u = x - shoulderOffsetX;
	float v = y - shoulderOffsetY;

	float s = atan2(v, u) - _shoulderOffsetAngle - _hexapod->yaw - _yawOffset;

	if (s < g->shoulderMinAngle) s += 2*M_PI;
	if (s > g->shoulderMaxAngle) s -= 2*M_PI;
	shoulderNormalized = (s - g->shoulderMinAngle) / g->shoulderAngleSpan();

	float tw = g->tipToWrist, wk = g->wristToKnee;

	float P = sqrt(u * u + v * v) - g->kneeToShoulder;
	float Q = z - g->kneeToBottom;
	float SS = P*P + Q*Q;

	float w = M_PI - aCosSaturated((tw*tw + wk*wk - SS) / tw / wk / 2);
	if (w < g->wristMinAngle) w += 2*M_PI;
	while (w > g->wristMaxAngle) w -= 2*M_PI;
	if (w < g->wristMinAngle) {
		wristNormalized = (fabsf(tan(w - g->wristMinAngle)) > fabsf(tan(w - g->wristMaxAngle))) ? 1 : 0;
    } else {
		wristNormalized = (w - g->wristMinAngle) / g->wristAngleSpan();
	}

	float a = aCosSaturated((wk*wk + SS - tw*tw) / wk / sqrt(SS) / 2);
	float b = atan2(fabsf(P), fabsf(Q));
	float k = M_PI - a - b;
	if (k < g->kneeMinAngle) k += 2*M_PI;
	while (k > g->kneeMaxAngle) k -= 2*M_PI;
	if (k < g->kneeMinAngle) {
		kneeNormalized = (fabsf(tan(k - g->kneeMinAngle)) > fabsf(tan(k - g->kneeMaxAngle))) ? 1 : 0;
    } else {
        kneeNormalized = (k - g->kneeMinAngle) / g->kneeAngleSpan();
	}
}
/*
void Limb::moveToStanceDelta(float dx, float dy, float dz, uint16_t time)
{
	float K, W, S;
	currentCoordsX = stanceX + dx;
	currentCoordsY = stanceY + dy;
	currentCoordsZ = stanceZ + dz;
	coorsToNormalized(currentCoordsX, currentCoordsY, currentCoordsZ, K, W, S);
	//std::cout << "Delta (" << dx << ", " << dy << ", " << dz << ") : (" << K << ", " << W << ", " << S << ")\n";
	moveToPWMWidth(_knee->pwmWidthForNormalizedPosition(K),
                   _wrist->pwmWidthForNormalizedPosition(W),
                   _shoulder->pwmWidthForNormalizedPosition(S), time);
}

void Limb::moveToPWMWidth(uint16_t kneeWidth, uint16_t wristWidth, uint16_t shoulderWidth, const uint16_t time)
{
    currentKneeWidth = kneeWidth;
    currentWristWidth = wristWidth;
    currentShoulderWidth = shoulderWidth;

    //std::cout << "PWM (" << kneeWidth << ", " << wristWidth << ", " << shoulderWidth << ")\n";

    char b[12];

	b[0] = 0x80 + _knee->id();
	b[1] = kneeWidth >> 8;
	b[2] = kneeWidth & 0xFF;
	b[3] = 0x80 + _wrist->id();
	b[4] = wristWidth >> 8;
	b[5] = wristWidth & 0xFF;
	b[6] = 0x80 + _shoulder->id();
	b[7] = shoulderWidth >> 8;
	b[8] = shoulderWidth & 0xFF;
	b[9] = 0xA1;
	b[10] = time >> 8;
	b[11] = time & 0xFF;

	if (enabled) Serial::send(b, 12);
}
*/
void Limb::updateCurrentPositions()
{
    prevCoordsX = currentCoordsX;
    prevCoordsY = currentCoordsY;
    prevCoordsZ = currentCoordsZ;
    normalizedToCoords(_knee->currentPosition, _wrist->currentPosition, _shoulder->currentPosition,
                       currentCoordsX, currentCoordsY, currentCoordsZ);
}
