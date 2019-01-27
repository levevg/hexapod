#include "Joint.h"
#include "Serial.h"
#include "Config.h"
#include <iostream>

Joint::Joint(uint8_t servoId, float minServoDelay, float maxServoDelay)
{
    channelId = servoId;
    minDelay = minServoDelay;
	maxDelay = maxServoDelay;
    span = maxServoDelay - minServoDelay;
    offset = minServoDelay;
}

void Joint::move(float normalizedPosition, uint16_t movingTime)
{
	unsigned char b[6];
	uint16_t pwmWidth = pwmWidthForNormalizedPosition(normalizedPosition);
    //std::cout << "Move #" << (int)channelId << " " << normalizedPosition << " (" << pwmWidth << ")\n";
	b[0] = 0x80 + channelId;
	b[1] = pwmWidth >> 8;
	b[2] = pwmWidth & 0xFF;
	b[3] = 0xA1;
	b[4] = movingTime >> 8;
	b[5] = movingTime & 0xFF;
	servosPort.send(b, 6);
}

uint16_t Joint::pwmWidthForNormalizedPosition(float normalized)
{
    return span * normalized + offset;
}
