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

#include "version.h"

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


/* --- GEOMETRY --- */

typedef struct {
    int x, y;
} pt;

struct main_opts {
    u64 random_seed;
    u32 columns;
    u32 rows;

    u32 corridor_width;
    u32 pen_radius;

    const char *fg_color;
    const char *output;
};


/* --- GRIDS --- */

typedef struct {
    u32 columns;
    u32 rows;
    u8 *cells;
} grid;

grid* grid_alloc_init(const u32 columns, const u32 rows, const u8 initval) {
    grid *g = malloc(sizeof(grid));
    if (g == NULL) {
        fprintf(stderr, "Unable to allocate memory for grid struct\n");
        return NULL;
    }

    g->cells = malloc(sizeof(u8) * columns * rows);
    if (g->cells == NULL) {
        fprintf(stderr, "Unable to allocate memory for %ux%u grid cells\n", columns, rows);
        return NULL;
    }

    g->columns = columns;
    g->rows = rows;

    memset(g->cells, initval, rows * columns);
    return g;
}

void grid_free(grid *grid) {
    if (grid != NULL) {
        free(grid->cells);
        free(grid);
    }
}

/* Temp function for testing: print fg char for 1, bg char for 0. */
void grid_print(grid *grid, const char *fg, const char *bg) {
    for (u32 y = 0, y_ = grid->rows; y < y_; ++y) {
        for (u32 x = 0, x_ = grid->columns; x < x_; ++x) {
            printf("%s", (grid->cells[y * x_ + x]) ? fg : bg);
        }
        printf("\n");
    }
}


/* --- MAZE GENERATOR --- */

/**
 * Recursively wander around the grid at random, stopping at any cells that is
 * already visited or out of bounds.
 *
 * For any newly visited cell whose coordinate is given by `curr`: Mark it
 * visited, then carve out the corresponding space from `maze_grid` as well as
 * the cell connecting it to the space we came from, given by `prev`.
 *
 * The walk finishes once all cells are visited.
 */
void maze_visit(grid *walk_grid, grid *maze_grid, pt curr, pt prev) {
    /* OOB check, base case */
    if (curr.x < 0 || curr.x >= (int)walk_grid->columns ||
        curr.y < 0 || curr.y >= (int)walk_grid->rows) {
        return;
    }
    /* Already visited, base case */
    if (walk_grid->cells[curr.y * walk_grid->columns + curr.x]) {
        return;
    }

    /* Mark seen */
    walk_grid->cells[curr.y * walk_grid->columns + curr.x] = 1;

    /* Carve from this cell to where we just came from in the maze */
    pt dir = {prev.x - curr.x, prev.y - curr.y};
    pt maze_curr = {curr.x * 2 + 1, curr.y * 2 + 1};
    pt maze_prev = {maze_curr.x + dir.x, maze_curr.y + dir.y};
    maze_grid->cells[maze_curr.y * maze_grid->columns + maze_curr.x] = 0;
    maze_grid->cells[maze_prev.y * maze_grid->columns + maze_prev.x] = 0;

    /* Now do the same for neighbouring cells in a shuffled order */
    pt directions[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    u8 done[4] = {0, 0, 0, 0};
    while (!done[0] || !done[1] || !done[2] || !done[3]) {
        u32 r = pcg32_nextuint(&srng) % 4; /* @fixme Not great shuffle */
        while (done[r]) {
            r = pcg32_nextuint(&srng) % 4;
        }
        done[r] = 1;
        dir = directions[r];
        maze_visit(walk_grid, maze_grid, (pt){curr.x + dir.x, curr.y + dir.y}, curr);
    }
}

/**
 * Initiallize a grid that can hold a generated maze of `columns` x `rows`
 * corridors. (N.B: That is columns x rows walkable space; including walls the
 * actual grid size will be 2n + 1 in each dimension.)
 *
 * This returns a pointer to a newly generated maze grid. It is the
 * responsibility of the caller to free the grid when done.
 *
 * @return Grid* Pointer to a grid containing the generated maze.
 */
grid* generate_maze(u32 columns, u32 rows) {
    /* Initialize two boolean grids: One to track the progress of the random
     * walk, the other to carve out the paths the walker has visited level
     * walls behind.
     */
    grid *walk_grid = grid_alloc_init(columns, rows, 0);
    grid *maze_grid = grid_alloc_init(columns * 2 + 1, rows * 2 + 1, 1);

    /* Start at a random point: */
    pt start = {pcg32_nextuint(&srng) % columns, pcg32_nextuint(&srng) % rows};

    maze_visit(walk_grid, maze_grid, start, start);

    /* Done with the random walk. */
    grid_free(walk_grid);
    return maze_grid;
}


/* --- SVG Output --- */

void draw_svg_maze(grid *grid, struct main_opts *opts) {
    /* Calculate total width and height: */
    u32 total_width = (grid->columns / 2) * opts->corridor_width;
    u32 total_height = (grid->rows / 2) * opts->corridor_width;

    /* SVG Preamble */
    printf("<?xml version='1.0' standalone='no'?>\n");
    printf("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 %u %u'>\n",
           total_width, total_height);
    printf("<g stroke-linecap='round' stroke-width='%u' stroke='%s'>\n",
           opts->pen_radius, opts->fg_color);

    u32 ypos = 0;
    for (u32 y = 0, y_ = grid->rows; y < y_; y += 2) {
        u32 x1 = 0;
        u32 x2 = 0;
        for (u32 x = 0, x_ = grid->columns; x < x_;) {
            while (x < x_ && grid->cells[y * x_ + x]) {
                if (x % 2 != 0) {
                    x2 += opts->corridor_width;
                }
                ++x;
            }

            if (x2 > x1) {
                printf(" <line x1='%u' y1='%u' x2='%u' y2='%u'/>\n",
                       x1, ypos, x2, ypos);
            }

            while (x < x_ && !grid->cells[y * x_ + x]) {
                x2 += opts->corridor_width;
                x1 = x2;
                ++x;
            }
        }

        ypos += opts->corridor_width;
    }

    u32 xpos = 0;
    for (u32 x = 0, x_ = grid->columns; x < x_; x += 2) {
        u32 y1 = 0;
        u32 y2 = 0;
        for (u32 y = 0, y_ = grid->rows; y < y_;) {
            while (y < y_ && grid->cells[y * x_ + x]) {
                if (y % 2 != 0) {
                    y2 += opts->corridor_width;
                }
                ++y;
            }

            if (y2 > y1) {
                printf(" <line x1='%u' y1='%u' x2='%u' y2='%u'/>\n",
                       xpos, y1, xpos, y2);
            }

            while (y < y_ && !grid->cells[y * x_ + x]) {
                y2 += opts->corridor_width;
                y1 = y2;
                ++y;
            }
        }

        xpos += opts->corridor_width;
    }

    /* SVG Close */
    printf("</g>\n");
    printf("</svg>\n");
}


/* --- PROGRAM --- */

u64 strhash(const char *const str) {
    u64 hash = 57;

    for (u32 k = 0; str[k] != '\0'; ++k) {
        hash = 37 * hash + str[k];
    }

    return hash;
}

int main(int argc, char *argv[]) {

    /* Option Defaults */
    struct main_opts opts = {
        .random_seed = PCG32_INITSTATE,
        .columns = 8,
        .rows = 8,

        .corridor_width = 5,
        .pen_radius = 1,
        .fg_color = "black",
        .output = "ascii",
    };

    /* Process arguments: */
    u8 args = 1;
    for (int k = 1; args && (k < argc); ++k) {
        const char *arg = argv[k];

        if (*arg++ != '-')
            goto usage;

        switch (*arg++) {
        case 'r':              /* Random Number Seed.  */
            if (!*arg)
                goto usage;

            opts.random_seed = strhash(arg);
            continue;

        case 'w':              /* Set Width  */
            if (!*arg)
                goto usage;

            opts.columns = (u32)strtoul(arg, NULL, 10);
            continue;

        case 'h':              /* Set Height  */
            if (!*arg)
                goto usage;

            opts.rows = (u32)strtoul(arg, NULL, 10);
            continue;

        case 'c':              /* Set Corridor width (SVG output) */
            if (!*arg)
                goto usage;

            opts.corridor_width = (u32)strtoul(arg, NULL, 10);
            continue;

        case 'p':              /* Set Pen radius (SVG output)  */
            if (!*arg)
                goto usage;

            opts.pen_radius = (u32)strtoul(arg, NULL, 10);
            continue;

        case 'o':              /* Set Output  */
            if (!*arg)
                goto usage;

            opts.output = arg;
            continue;

        case 'f':              /* Set Foreground Color (CSS Color string)  */
            if (!*arg)
                goto usage;

            opts.fg_color = arg;
            continue;

        case '-':              /* End of arguments.    */
            argv[k] = argv[0];
            argc = argc - k;
            argv = argv + k;
            args = 0;
            break;

        case 'v':              /* Show version and exit.      */
            puts(APPMETA_NAME " v" APPMETA_VERSION);
#ifndef NDEBUG
            puts("Build: " APPMETA_BUILD_DATE);
            puts("SCM: ("APPMETA_GIT_SHA1 ") " APPMETA_GIT_SUBJECT);
#endif /* NDEBUG */
            return 0;

        default:
 usage:
            puts(APPMETA_NAME " Options:");
            puts("  -v       - Show version and exit");
            puts("  -w       - Set maze width (columns)");
            puts("  -h       - Set maze height (rows)");
            puts("  -r       - Set random seed");
            puts("  -o<fmt>  - Set output format (svg|ascii)");
            return EXIT_FAILURE;
        }

        if (*arg)
            goto usage;
    }

    pcg32_srand(&srng, opts.random_seed);

    grid *maze = generate_maze(opts.columns, opts.rows);

    if (0 == strcmp("svg", opts.output)) {
        draw_svg_maze(maze, &opts);
    } else {
        grid_print(maze, "#", " ");
    }

    grid_free(maze);
    return 0;
}
