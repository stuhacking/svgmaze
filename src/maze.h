/**
 * @brief Maze generator
 */
#ifndef MAZE_H
#define MAZE_H

#include "types.h"
#include "grid.h"

struct svg_opts {
    u32 pen_radius;
    u32 corridor_width;

    const char *fg_color;
};

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
grid* maze_generate(u32 columns, u32 rows);

/**
 * Draw grid to the console as ASCII (or UTF-8 if the terminal will render it)
 * characters. Wall cells will be rendered as the `fg` glyph, spaces as the
 * `bg` glyph.
 */
void maze_draw_ascii(grid* maze, const char *fg, const char *bg);

/**
 * Draw grid to the console as an SVG document. Walls will be draw as a set of
 * lines using `opts.pen_radius` as the stroke width in pixels and
 * `opts.fg_color` as the stroke colour. Spacing between maze lines is given
 * by `opts.corridor_width` in pixels.
 */
void maze_draw_svg(grid* maze, struct svg_opts *opts);

#endif /* MAZE_H */
