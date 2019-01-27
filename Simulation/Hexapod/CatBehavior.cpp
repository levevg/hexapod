#include "Behavior.h"
#include "CatBehavior.h"
#include <cstdlib>

CatBehavior::CatBehavior(Hexapod *hexapod) : Behavior(hexapod) {
    dotX = dotY = lookDir = 0;
}

void CatBehavior::born() {
    _alive = true;
    setState(catRaise);

    dotX = 10;
}

void CatBehavior::die() {
    _alive = false;
}

void CatBehavior::fsm(uint32_t stateTime) {
    switch (_state) {

        case catRaise:
            _hexapod->raiseSequence();
            delayMs(500);
            setState(catIdle);
            break;

        case catIdle:
            if (NZ(dotX)) {
                setState(catRunForward);
            } else {
                if ((rand() & 127)*100 + 5000 < stateTime) {
                    int r = rand() & 3;
                    if (r == 0) {
                        setState(catBattlePose);
                    } else {
                        setState(catGlimpseAround);
                    }
                }
            }
            if (_state == catIdle) delayMs(200);
            break;

        case catRunForward:
            if (NZ(dotX)) {
                if (dotX < -40) moveA = dotX >> 1;
                if (dotX > 40) moveA = dotX >> 1;
                if (dotX > 90 || dotX < -90) moveY = 10; else moveY = 30;
            } else {
                if (prevMoveA > 40) setState(catLostToLeft);
                else if (prevMoveA < -40) setState(catLostToRight);
                else setState(catIdle);
            }
            break;

        case catLostToLeft:
            lookDir = 80;
            setState(catLookAround);
            break;

        case catLostToRight:
            lookDir = -80;
            setState(catLookAround);
            break;

        case catGlimpseAround:
            _hexapod->yaw = 0.8;
            _hexapod->moveAllToStanceDelta(0, 0, 0, 1000);
            delayMs(2500);
            if (NZ(dotX)) { setState(catGlimpseSucceed); break; }

            _hexapod->yaw = -0.8f;
            _hexapod->moveAllToStanceDelta(0, 0, 0, 2000);
            delayMs(3500);
            if (NZ(dotX)) { setState(catGlimpseSucceed); break; }

            _hexapod->yaw = 0;
            _hexapod->moveAllToStanceDelta(0, 0, 0, 1000);
            delayMs(2500);

            setState(catIdle);
            break;

        case catGlimpseSucceed:
            _hexapod->moveOddToStanceDelta(0, 0, 35, 300);
            delayMs(300);

            _hexapod->yaw = 0;
            _hexapod->moveOddToStanceDelta(0, 0, 30, 300);
            delayMs(300);

            _hexapod->moveOddToStanceDelta(0, 0, 0, 300);
            delayMs(300);

            _hexapod->moveEvenToStanceDelta(0, 0, 35, 300);
            delayMs(300);

            _hexapod->yaw = 0;
            _hexapod->moveEvenToStanceDelta(0, 0, 30, 300);
            delayMs(300);

            _hexapod->moveEvenToStanceDelta(0, 0, 0, 300);
            delayMs(1000);

            setState(catIdle);
            break;

        case catLostToBottom:
            break;

        case catLookAround:
            if (NZ(dotX)) {
                setState(catRunForward);
            } else {
                moveA = lookDir;
                if ((rand() & 3) == 0) lookDir = -lookDir;
                if ((rand() & 7) == 0) setState(catIdle);
            }
            break;

        case catBattlePose:
            _hexapod->battlePoseSequence();
            setState(catIdle);
            break;
    }
}
