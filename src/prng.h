/**
 * @file
 * @brief Pseudo-random number generator
 *
 * SPDX-FileCopyrightText: 2014 M.E. O'Neill
 * SPDX-License-Identifier: Apache-2.0
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
