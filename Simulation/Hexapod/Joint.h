#ifndef JOINT_H
#define JOINT_H

#include <cstdint>

class Joint
{
    public:
        Joint(uint8_t servoId, float minServoDelay, float maxServoDelay);

        void move(float normalizedPosition, uint16_t movingTime);
        uint16_t pwmWidthForNormalizedPosition(float normalized);

        uint8_t id() { return channelId; };
    private:
        uint8_t channelId;
        float minDelay;
        float maxDelay;
        float offset;
        float span;
};

#endif // JOINT_H
