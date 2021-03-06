#ifndef TABLE_DEFINITION_H

#define TABLE_DEFINITION_H

#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>


#define PAGE_SIZE 4096
#define TABLE_MAX_PAGES 100

typedef struct {
  int file_descriptor;
  uint32_t file_length;
  uint32_t num_pages;
  void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
  Pager* pager;
  uint32_t root_page_num;
} Table;

typedef struct {
  Table* table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table;
} Cursor;

#include "row_definition.h"
#include "b-tree.h"

Pager* pager_open(const char* filename);

void* get_page(Pager* pager, uint32_t page_num);

void db_close(Table* table);

void pager_flush(Pager* pager, uint32_t page_num);

NodeType get_node_type(void* node);

Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);

void* cursor_value(Cursor* cursor)
{
  uint32_t page_num = cursor->page_num;
  void* page = get_page(cursor->table->pager, page_num);

  return leaf_node_value(page, cursor->cell_num);
}

void cursor_advance(Cursor* cursor)
{
  uint32_t page_num = cursor->page_num;
  void* node = get_page(cursor->table->pager, page_num);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*leaf_node_num_cells(node)))
  {
    cursor->end_of_table = true;
  }
}

Table* db_open(const char* filename)
{
  Pager* pager = pager_open(filename);

  Table* table = malloc(sizeof(Table));
  table->pager = pager;
  table->root_page_num = 0;

  if ( pager->num_pages == 0 )
  {
    // New database file. Initialize page 0 as leaf node.
    void* root_node = get_page(pager, 0);
    initialize_leaf_node(root_node);
  }

  return table;
}


void db_close(Table* table)
{
  Pager* pager = table->pager;

  for (uint32_t i = 0; i < pager->num_pages; i++)
  {
    if (pager->pages[i] == NULL)
    {
      continue;
    }
    pager_flush(pager, i);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  int result = (pager->file_descriptor);
  if (result == -1)
  {
    printf("Error closing db file.\n");
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
  pager->num_pages = file_length / PAGE_SIZE;

  if ( file_length % PAGE_SIZE != 0)
  {
    printf("DB file is not a whole number of pages. Corrupt file.\n");
    exit(EXIT_FAILURE);
  }
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
  {
    pager->pages[i] = NULL;
  }
  return pager;
}

void pager_flush(Pager* pager, uint32_t page_num)
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
  
  ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);
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

    if (page_num >= pager->num_pages)
    {
      pager->num_pages = page_num + 1;
    }
  }
  return pager->pages[page_num];
}

Cursor* table_start(Table* table)
{
  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = table->root_page_num;
  cursor->cell_num = 0;

  void* root_node = get_page(table->pager, table->root_page_num);
  uint32_t num_cells = *leaf_node_num_cells(root_node);
  cursor->end_of_table = num_cells == 0;
  return cursor;
}

/* Return the position of the given key.
 * If the key is not present, return the position
 * where it should be inserted
 * */

Cursor* table_find(Table* table, uint32_t key)
{
  uint32_t root_page_num = table->root_page_num;
  void* root_node = get_page(table->pager, root_page_num);
  
  if (get_node_type(root_node) == NODE_LEAF)
  {
    return leaf_node_find(table, root_page_num, key);
  }
  else
  {
    printf("Need to implmenent searching an internal node \n");
    exit(EXIT_FAILURE);
  }
}

void print_constants() {
  printf("ROW_SIZE: %ld\n", ROW_SIZE);
  printf("COMMON_NODE_HEADER_SIZE: %ld\n", COMMON_NODE_HEADER_SIZE);
  printf("LEAF_NODE_HEADER_SIZE: %ld\n", LEAF_NODE_HEADER_SIZE);
  printf("LEAF_NODE_CELL_SIZE: %ld\n", LEAF_NODE_CELL_SIZE);
  printf("LEAF_NODE_SPACE_FOR_CELLS: %ld\n", LEAF_NODE_SPACE_FOR_CELLS);
  printf("LEAF_NODE_MAX_CELLS: %ld\n", LEAF_NODE_MAX_CELLS);
}

void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value)
{
  void* node = get_page(cursor->table->pager, cursor->page_num);
 
  uint32_t num_cells = *leaf_node_num_cells(node);

  if ( num_cells >= LEAF_NODE_MAX_CELLS )
  {
    // Node full
    printf("Need to implement splitting a leaf node. \n");
    exit(EXIT_FAILURE);
  }

  if (cursor->cell_num < num_cells)
  {
    // Make room for new cell
    for (uint32_t i = num_cells; i > cursor->cell_num; i--)
    {
      memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
    }

  }
  *(leaf_node_num_cells(node)) += 1;
  *(leaf_node_key(node, cursor->cell_num)) = key;

  serialize_row(value, leaf_node_value(node, cursor-> cell_num));

}

void print_leaf_node(void* node)
{
  uint32_t num_cells = *leaf_node_num_cells(node);
  printf("Leaf (size %d)\n", num_cells);
  for (uint32_t i = 0; i < num_cells; i++)
  {
    uint32_t key = *leaf_node_key(node, i);
    printf(" - %d : %d\n", i, key);
  }
}

NodeType get_node_type(void* node)
{
  uint8_t value = *((uint8_t*)(node + NODE_TYPE_OFFSET));
  return (NodeType)value;
}


Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key)

{
  void* node = get_page(table->pager, page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);

  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = page_num;

  // Binary search
  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;

  while(one_past_max_index != min_index)
  {
    uint32_t index = (min_index + one_past_max_index ) / 2;
    uint32_t key_at_index = *leaf_node_key(node, index);
    if (key == key_at_index)
    {
      cursor->cell_num = index;
      return cursor;
    }
    if (key < key_at_index)
    {
      one_past_max_index = index;
    }
    else
    {
      min_index = index + 1;
    }
  }
  cursor->cell_num = min_index;
  return cursor;
}

#endif
