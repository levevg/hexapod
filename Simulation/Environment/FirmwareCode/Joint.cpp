#include "Joint.h"
#include "Serial.h"
#include <iostream>

Joint::Joint(uint8_t servoId, float minServoDelay, float maxServoDelay)
{
    channelId = servoId;
    minDelay = minServoDelay;
	maxDelay = maxServoDelay;
    span = maxServoDelay - minServoDelay;
    offset = minServoDelay;
}

uint16_t Joint::pwmWidthForNormalizedPosition(float normalized)
{
    return span * normalized + offset;
}

void Joint::updateCurrentPosition(float time) {
	if (movingEndTime < time) {
		currentPosition = movingEndPosition;
		return;
	}
	currentPosition = movingStartPosition + (time - movingStartTime) / (movingEndTime - movingStartTime) * (movingEndPosition - movingStartPosition);
}
