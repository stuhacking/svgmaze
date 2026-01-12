/** @brief PRNG implementation */
#include "prng.h"

/* Internally, the PRNG is based on the minimal PCG32 implementation: */

/* *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
   Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
*/
static const u64 PCG32_INITSTATE = 0x853c49e6748fea9b;
static const u64 PCG32_INITINC = 0xda3e39cb94b95bdb;

struct pcg32_state {
    u64 state;
    u64 inc;
};

static u32 pcg32_nextuint(struct pcg32_state *rng) {
    u64 oldstate = rng->state;
    /* Advance internal state: */
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
    /* Calculate output function (XSH RR), uses old state for max ILP */
    u32 xorshifted = ((oldstate >> 18U) ^ oldstate) >> 27U;
    u32 rot = oldstate >> 59U;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static void pcg32_srand(struct pcg32_state *rng, u64 state, u64 inc) {
    rng->state = state;
    rng->inc = inc;
}


/* --- PRNG API --- */

static struct pcg32_state srng = {
    .state = PCG32_INITSTATE,
    .inc = PCG32_INITINC
};

void prng_srand(u64 seed) {
    pcg32_srand(&srng, seed, PCG32_INITINC);
}

u64 prng_nextuint(void) {
    return pcg32_nextuint(&srng);
}
