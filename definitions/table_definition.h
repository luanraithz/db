#ifndef TREE_DEFINITION.H

#include <stdint.h>
#include "row_definition.h"

#define TREE_DEFINITION.H

#define PAGE_SIZE 4096;
#define TABLE_MAX_PAGES 100;
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_PAGES)

typedef struct {
	void* pages[TABLE_MAX_PAGES];
	uint32_t num_rows;
} Table

#endif