#ifndef TABLE_DEFINITION_H

#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "b-tree.h"
#include "row_definition.h"

#define TABLE_DEFINITION_H

#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_PAGES)

typedef struct {
	int file_descriptor;
	uint32_t file_length;
	void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
	Pager* pager;
	uint32_t num_rows;
} Table;

typedef struct {
	Table* table;
	uint32_t row_num;
	bool end_of_table;
} Cursor;


Pager* pager_open(const char* filename);

void* get_page(Pager* pager, uint32_t page_num);

void db_close(Table* table);

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);

void* cursor_value(Cursor* cursor)
{
	uint32_t row_num = cursor->row_num;
	uint32_t page_num = row_num / ROWS_PER_PAGE;
	void* page = get_page(cursor->table->pager, page_num);
	uint32_t row_offset = row_num % ROWS_PER_PAGE;
	uint32_t byte_offset = row_offset * ROW_SIZE;
	return page + byte_offset;
}

void cursor_advance(Cursor* cursor)
{
	cursor->row_num += 1;
	if (cursor->row_num >= cursor->table->num_rows)
	{
		cursor->end_of_table = true;
	}
}
Table* db_open(const char* filename)
{
	Pager* pager = pager_open(filename);
	uint32_t num_rows = pager->file_length / ROW_SIZE;

	Table* table = malloc(sizeof(Table));
	table->pager = pager;
	table->num_rows = num_rows;
	return table;
}


void db_close(Table* table)
{
	Pager* pager = table->pager;
	uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

	for (uint32_t i = 0; i < num_full_pages; i++)
	{
		if (pager->pages[i] == NULL)
		{
			continue;
		}
		pager_flush(pager, i, PAGE_SIZE);
		free(pager->pages[i]);
		pager->pages[i] = NULL;
	}

	// There may be a partial page to write to the end of the file
	// This should not be needed after we switch to a B-tree
	uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;

	if (num_additional_rows > 0)
	{
		uint32_t page_num = num_full_pages;
		if (pager->pages[page_num] != NULL)
		{
			pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
			free(pager->pages[page_num]);
			pager->pages[page_num] = NULL;
		}
	}
	free(pager);
}

Pager* pager_open(const char* filename)
{
	int fd = open(filename,
			O_RDWR | O_CREAT,
			S_IWUSR | S_IRUSR
		);
	if (fd == -1) {
		printf("Unable to open file;\n");
		exit(EXIT_FAILURE);
	}
	off_t file_length = lseek(fd, 0, SEEK_END);

	Pager* pager = malloc(sizeof(Pager));
	pager->file_descriptor = fd;
	pager->file_length = file_length;
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
	{
		pager->pages[i] = NULL;
	}
	return pager;
}

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size)
{
	if (pager->pages[page_num] == NULL)
	{
		printf("Tried to flush null page\n");
		exit(EXIT_FAILURE);
	}

	off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

	if (offset == -1)
	{
		printf("Error seeking: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);

	if (bytes_written == -1)
	{
		printf("Error writing: %d\n", errno);
		exit(EXIT_FAILURE);
	}
}


void* get_page(Pager* pager, uint32_t page_num)
{
	if (page_num > TABLE_MAX_PAGES)
	{
		printf("Tried to fetch page number out of bounds, %d.\n", TABLE_MAX_PAGES);
		exit(EXIT_FAILURE);
	}
	if (pager->pages[page_num] == NULL)
	{
		// Cache miss. Allocate memory and load from file.
		void* page = malloc(PAGE_SIZE);
		uint32_t num_pages = pager->file_length / PAGE_SIZE;

		// We migh save a partial page at the end of the file
		if (pager->file_length % PAGE_SIZE)
		{
			num_pages+= 1;
		}

		if (page_num <= num_pages)
		{
			lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
			ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
			if (bytes_read == -1)
			{
				printf("Error reading file: %d\n", errno);
				exit(EXIT_FAILURE);
			}
		}

		pager->pages[page_num] = page;
	}
	return pager->pages[page_num];
}

Cursor* table_start(Table* table)
{
	Cursor* cursor = malloc(sizeof(Cursor));
	cursor->table = table;
	cursor->row_num = 0;
	cursor->end_of_table = (table->num_rows == 0);
	return cursor;	
}

Cursor* table_end(Table* table)
{
	Cursor* cursor = malloc(sizeof(Cursor));
	cursor->table = table;
	cursor->row_num = table->num_rows;
	cursor->end_of_table = true;

	return cursor;
}



#endif
