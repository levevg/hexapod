#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

#define EPSILON 0.00001

#define NZ(f) (((f) <= -EPSILON) || ((f) >= EPSILON))
#define IZ(f) (((f) > -EPSILON) && ((f) < EPSILON))

void delayMs(uint16_t delay);
uint64_t mstime();

#endif // UTILS_H
