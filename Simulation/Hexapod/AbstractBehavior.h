#ifndef ABSTRACTBEHAVIOR_H
#define ABSTRACTBEHAVIOR_H

#include "Hexapod.h"
#include <cstdint>

class AbstractBehavior
{
    public:
        explicit AbstractBehavior(Hexapod *hexapod);

        virtual void born() = 0;
        virtual bool live() = 0;
        virtual void die() = 0;

    protected:
        Hexapod *_hexapod;
        bool _alive;
        uint64_t _stateTimestamp;
};

#endif // ABSTRACTBEHAVIOR_H
