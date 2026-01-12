/**
 * @brief Maze Generator with SVG output main file.
 *
 * For simple rectangular maze generation, we will initially generate
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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "version.h"
#include "types.h"
#include "strings.h"
#include "prng.h"
#include "grid.h"
#include "maze.h"


struct main_opts {
    u64 random_seed;
    u32 columns;
    u32 rows;

    u32 corridor_width;
    u32 pen_radius;

    const char *fg_color;
    const char *output;
};


int main(int argc, char *argv[]) {

    /* Option Defaults */
    struct main_opts opts = {
        .random_seed = 1,
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
            puts("  -w<n>    - Set maze width (columns)");
            puts("  -h<n>    - Set maze height (rows)");
            puts("  -r<s>    - Set random seed (string)");
            puts("  -o<fmt>  - Set output format (svg|ascii, default ASCII)");
            puts("  -c<n>    - Set corridor width (pixels, SVG output)");
            puts("  -p<n>    - Set pen radius (pixels, SVG output)");
            puts("  -f<s>    - Set foreground colour (CSS Color3 string)");
            return 1;
        }

        if (*arg)
            goto usage;
    }

    prng_srand(opts.random_seed);

    grid *maze = maze_generate(opts.columns, opts.rows);

    if (0 == strcmp("svg", opts.output)) {
        struct svg_opts svg_opts = {
            .pen_radius = opts.pen_radius,
            .corridor_width = opts.corridor_width,
            .fg_color = opts.fg_color,
        };
        maze_draw_svg(maze, &svg_opts);
    } else {
        maze_draw_ascii(maze, "#", " ");
    }

    grid_free(maze);
    return 0;
}
