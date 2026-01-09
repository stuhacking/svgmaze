/**
 * @brief Maze Generator with SVG output main file.
 *
 * As per the typical approach to maze generation, we will initially generate
 * a grid that is 2n+1 the dimensions of width x height. Each even row or
 * column (zero indexed) represents where the walls will be, and the odds are
 * therefore the corridors. i.e:
 *              123456789
 *            1 +-+-+-+-+
 *            2   | |   |
 *            3 + + +-+ +
 *            4 |     |
 *            5 +-+-+ + +
 *            6 |       |
 *            7 +-+-+-+-+
 *
 * This maze essentially starts off fully filled in, then a random walker
 * bounded by dimensions width x height will carve out space as it traverses
 * every cell. Finally a couple of cells around the boundary can be removed if
 * entry/exits are desired, since the maze is fully explored, any two boundary
 * cells should be connected though the difficulty of the maze may vary.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;


/* --- PRNG --- */

/* *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
   Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
*/
#define PCG32_INITSTATE 0x853c49e6748fea9bULL
#define PCG32_INITINC   0xda3e39cb94b95bdbULL

struct pcg32_state {
    u64 state;
    u64 inc;
};

static struct pcg32_state srng;

u32 pcg32_nextuint(struct pcg32_state *rng) {
    u64 oldstate = rng->state;
    /* Advance internal state: */
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
    /* Calculate output function (XSH RR), uses old state for max ILP */
    u32 xorshifted = ((oldstate >> 18U) ^ oldstate) >> 27U;
    u32 rot = oldstate >> 59U;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void pcg32_srand(struct pcg32_state *rng, u64 state) {
    rng->state = state;
    rng->inc = PCG32_INITINC;
}


/* --- PROGRAM --- */

int main(void) {
    pcg32_srand(&srng, PCG32_INITSTATE);

    printf("I don't do much, yet...\n");

    return 0;
}
