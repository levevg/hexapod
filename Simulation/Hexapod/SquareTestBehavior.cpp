#include "SquareTestBehavior.h"

SquareTestBehavior::SquareTestBehavior(Hexapod *hexapod) : Behavior(hexapod) {}

void SquareTestBehavior::born() {
    _alive = true;
    setState(stRaise);
}

void SquareTestBehavior::die() {
    _alive = false;
}

void SquareTestBehavior::fsm(uint32_t stateTime) {
    switch (_state) {
        case stRaise:
            _hexapod->raiseSequence();
            delayMs(500);
            setState(stGoForward);
            break;
        case stGoForward:
            if (stateTime > sideTime) {
                setState(stGoRight);
                break;
            }
            moveY = 30;
            break;
        case stGoLeft:
            if (stateTime > sideTime) {
                setState(stGoForward);
                break;
            }
            moveX = -30;
            break;
        case stGoBack:
            if (stateTime > sideTime) {
                setState(stGoLeft);
                break;
            }
            moveY = -30;
            break;
        case stGoRight:
            if (stateTime > sideTime) {
                setState(stGoBack);
                break;
            }
            moveX = 30;
            break;
    }
}

