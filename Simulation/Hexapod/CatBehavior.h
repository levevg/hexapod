#ifndef CATBEHAVIOR_H
#define CATBEHAVIOR_H
#include "Hexapod.h"
#include "Behavior.h"

typedef enum {
	catIdle,
	catRaise,
	catRunForward,
	catLostToLeft,
	catLostToRight,
	catLostToBottom,
	catLookAround,
	catGlimpseAround,
	catGlimpseSucceed,
	catBattlePose
} CatState;

class CatBehavior : public Behavior<CatState>
{
    public:
		explicit CatBehavior(Hexapod *hexapod);

        void born() override;
        void die() override;
		void fsm(uint32_t stateTime) override;

    private:
        int8_t dotX, dotY, lookDir;

};

#endif // CATBEHAVIOR_H
