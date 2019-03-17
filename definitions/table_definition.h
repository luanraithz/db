#ifndef TABLE_DEFINITION_H

#include <stdint.h>
#include "row_definition.h"

#define TABLE_DEFINITION_H

#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_PAGES)

typedef struct {
	void* pages[TABLE_MAX_PAGES];
	uint32_t num_rows;
} Table;

void* row_slot(Table* table, uint32_t row_num)
{
	uint32_t page_num = row_num / ROWS_PER_PAGE;
	void* page = table->pages[page_num];
	if (!page)
	{
		// Allocate memory only when we try to access the page
		page = table->pages[page_num] = malloc(PAGE_SIZE);
	}
	uint32_t row_offset = row_num % ROWS_PER_PAGE;
	uint32_t byte_offset = row_offset * ROW_SIZE;
	return page + byte_offset;
}

Table* new_table()
{
	Table* table = malloc(sizeof(Table));
	table->num_rows = 0;

	return table;
}

#endif
