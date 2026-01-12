/**
 * @brief Pseudo-random number generator
 *
 * Expose a simple interface for pseudorandom numbers.
 */
#ifndef PRNG_H
#define PRNG_H

#include "types.h"

/**
 * Seed an internal PRNG with a new initial seed.
 */
void prng_srand(u64 seed);

/**
 * Get the next unsigned random value.
 */
u64 prng_nextuint(void);

#endif /* PRNG_H */
