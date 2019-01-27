#ifndef HEXAPODFIRMWARE_SQUARETESTBEHAVIOR_H
#define HEXAPODFIRMWARE_SQUARETESTBEHAVIOR_H
#include "Hexapod.h"
#include "Behavior.h"

typedef enum {
    stRaise,
    stGoForward,
    stGoLeft,
    stGoBack,
    stGoRight
} SquareTestState;

class SquareTestBehavior : public Behavior<SquareTestState> {
    public:
        explicit SquareTestBehavior(Hexapod *hexapod);

        void born() override;
        void die() override;
        void fsm(uint32_t stateTime) override;

    private:
        uint32_t sideTime = 50000;
};

#endif //HEXAPODFIRMWARE_SQUARETESTBEHAVIOR_H
