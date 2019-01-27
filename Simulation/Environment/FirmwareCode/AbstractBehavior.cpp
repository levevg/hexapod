#include "AbstractBehavior.h"

AbstractBehavior::AbstractBehavior(Hexapod *hexapod) {
    _alive = false;
    _hexapod = hexapod;
    _stateTimestamp = 0;
}
