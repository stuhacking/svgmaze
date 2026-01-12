/**
 * @brief Grid functions
 */
#ifndef GRID_H
#define GRID_H

#include "types.h"

typedef struct {
    u32 columns;
    u32 rows;
    u8 *cells;
} grid;

/**
 * Allocate enough storage for a new grid of 8bit values of dimensions
 * `columns` x `rows`. Initialize all cells to `initval`.
 *
 * @return grid* Pointer to new allocated grid or NULL if allocation failed.
 */
grid* grid_alloc_init(const u32 columns,
                      const u32 rows,
                      const u8 initval);

/**
 * Free the memory allocated for a grid.
 */
void grid_free(grid *grid);

#endif /* GRID_H */
