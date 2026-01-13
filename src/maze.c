/** @brief Maze generator implementation */
#include "maze.h"

#include "prng.h"
#include <stdio.h>


typedef struct {
    int x, y;
} pt;


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
static void maze_visit(grid *walk_grid, grid *maze_grid, pt curr, pt prev) {
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
        u32 r = prng_nextuint() % 4; /* @fixme Not great shuffle */
        while (done[r]) {
            r = prng_nextuint() % 4;
        }
        done[r] = 1;
        dir = directions[r];
        maze_visit(walk_grid, maze_grid, (pt){curr.x + dir.x, curr.y + dir.y}, curr);
    }
}

grid* maze_generate(u32 columns, u32 rows) {
    /* Initialize two boolean grids: One to track the progress of the random
     * walk, the other to carve out the paths the walker has visited level
     * walls behind.
     */
    grid *walk_grid = grid_alloc_init(columns, rows, 0);
    grid *maze_grid = grid_alloc_init(columns * 2 + 1, rows * 2 + 1, 1);

    /* Start at a random point: */
    pt start = {prng_nextuint() % columns, prng_nextuint() % rows};

    maze_visit(walk_grid, maze_grid, start, start);

    /* Done with the random walk. */
    grid_free(walk_grid);
    return maze_grid;
}

/**
 * Print maze as ASCII or UTF-8 characters to standard output.
 */
void maze_draw_ascii(grid *maze, const char *fg, const char *bg) {
    for (u32 y = 0, y_ = maze->rows; y < y_; ++y) {
        for (u32 x = 0, x_ = maze->columns; x < x_; ++x) {
            printf("%s", (maze->cells[y * x_ + x]) ? fg : bg);
        }
        printf("\n");
    }
}

/**
 * Render maze as an SVG document.
 */
void maze_draw_svg(grid *maze, struct svg_opts *opts) {
    /* Calculate total width and height: */
    u32 total_width = (maze->columns / 2) * opts->corridor_width;
    u32 total_height = (maze->rows / 2) * opts->corridor_width;

    /* SVG Preamble */
    printf("<?xml version='1.0' standalone='no'?>\n");
    printf("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 %u %u'>",
           total_width, total_height);
    printf("<g stroke-linecap='round' stroke-width='%u' stroke='%s'>",
           opts->pen_radius, opts->fg_color);

    u32 ypos = 0;
    for (u32 y = 0, y_ = maze->rows; y < y_; y += 2) {
        u32 x1 = 0;
        u32 x2 = 0;
        for (u32 x = 0, x_ = maze->columns; x < x_;) {
            while (x < x_ && maze->cells[y * x_ + x]) {
                if (x % 2 != 0) {
                    x2 += opts->corridor_width;
                }
                ++x;
            }

            if (x2 > x1) {
                printf("<line x1='%u' y1='%u' x2='%u' y2='%u'/>",
                       x1, ypos, x2, ypos);
            }

            while (x < x_ && !maze->cells[y * x_ + x]) {
                x2 += opts->corridor_width;
                x1 = x2;
                ++x;
            }
        }

        ypos += opts->corridor_width;
    }

    u32 xpos = 0;
    for (u32 x = 0, x_ = maze->columns; x < x_; x += 2) {
        u32 y1 = 0;
        u32 y2 = 0;
        for (u32 y = 0, y_ = maze->rows; y < y_;) {
            while (y < y_ && maze->cells[y * x_ + x]) {
                if (y % 2 != 0) {
                    y2 += opts->corridor_width;
                }
                ++y;
            }

            if (y2 > y1) {
                printf("<line x1='%u' y1='%u' x2='%u' y2='%u'/>",
                       xpos, y1, xpos, y2);
            }

            while (y < y_ && !maze->cells[y * x_ + x]) {
                y2 += opts->corridor_width;
                y1 = y2;
                ++y;
            }
        }

        xpos += opts->corridor_width;
    }

    /* SVG Close */
    printf("</g>");
    printf("</svg>\n");
}
