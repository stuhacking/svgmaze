/** @brief Grid Implementation */
#include "grid.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


grid* grid_alloc_init(const u32 columns, const u32 rows,
                      const u8 initval) {
    grid *g = malloc(sizeof(grid));
    if (g == NULL) {
        fprintf(stderr, "Unable to allocate memory for grid struct\n");
        return NULL;
    }

    g->cells = malloc(sizeof(u8) * columns * rows);
    if (g->cells == NULL) {
        fprintf(stderr, "Unable to allocate memory for %ux%u grid cells\n",
                columns, rows);
        free(g);
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
