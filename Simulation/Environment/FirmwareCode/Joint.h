#ifndef JOINT_H
#define JOINT_H

#include <cstdint>

class Joint
{
    public:
        Joint(uint8_t servoId, float minServoDelay, float maxServoDelay);

        uint16_t pwmWidthForNormalizedPosition(float normalized);

        uint8_t id() { return channelId; };

        float movingStartTime = 0;
        float movingStartPosition = 0.5;
        float movingEndTime = 0;
        float movingEndPosition = 0.5;
        float currentPosition = 0.5;
        void updateCurrentPosition(float time);

        uint8_t channelId;
        float minDelay;
        float maxDelay;
        float offset;
        float span;
};

#endif // JOINT_H
