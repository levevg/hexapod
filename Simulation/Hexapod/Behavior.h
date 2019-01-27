#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "AbstractBehavior.h"
#include "Hexapod.h"
#include "utils.h"
#include <cstdint>

template <typename StateEnum>
class Behavior : public AbstractBehavior
{
    public:
        explicit Behavior(Hexapod *hexapod) : AbstractBehavior(hexapod) {  }

    protected:
        void setState(StateEnum st) {
            _state = st;
            _stateTimestamp = mstime();
        }

        bool live() override;
        virtual void fsm(uint32_t stateTime) = 0;

        StateEnum _state;

        int8_t moveX = 0;
        int8_t moveY = 0;
        int8_t moveA = 0;
        int8_t prevMoveX = 0;
        int8_t prevMoveY = 0;
        int8_t prevMoveA = 0;

};

template<typename StateEnum>
bool Behavior<StateEnum>::live() {
    if (!_alive) {
        born();
        return true;
    }

    moveX = 0;
    moveY = 0;
    moveA = 0;

    auto stateTime = static_cast<uint32_t>(mstime() - _stateTimestamp);

    fsm(stateTime);

    if (NZ(moveX) || NZ(moveY) || NZ(moveA)) {
        _hexapod->stepSequence(moveX, moveY, moveA, prevMoveX!=moveX || prevMoveY!=moveY || prevMoveA==moveA, false);
    } else {
        if (NZ(prevMoveX) || NZ(prevMoveY)|| NZ(prevMoveA)) {
            _hexapod->stepSequence(prevMoveX, prevMoveY, prevMoveA, false, true);
        }
    }

    prevMoveX = moveX;
    prevMoveY = moveY;
    prevMoveA = moveA;

    return true;
}

#endif // BEHAVIOR_H
