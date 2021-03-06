#ifndef VIRTUAL_MACHINE_H

#define VIRTUAL_MACHINE_H

#include "statements/statements.h"
#include "definitions/row_definition.h"
#include "definitions/table_definition.h"
#include "definitions/b-tree.h"

typedef enum {
  EXECUTE_SUCCESS,
  EXECUTE_TABLE_FULL,
  EXECUTE_DUPLICATE_KEY
} ExecuteResult;

ExecuteResult execute_insert(Statement* statement, Table* table);

ExecuteResult execute_select(Statement* statement, Table* table);

ExecuteResult execute_statement(Statement* statement, Table* table)
{

  switch (statement->type)
  {
    case (STATEMENT_INSERT):
      return execute_insert(statement, table);
    case (STATEMENT_SELECT):
      return execute_select(statement, table);
  }
  
}

ExecuteResult execute_insert(Statement* statement, Table* table)
{
  void* node = get_page(table->pager, table->root_page_num);
  uint32_t num_cells = (*leaf_node_num_cells(node));
  if (num_cells >= LEAF_NODE_MAX_CELLS)
  {
    return EXECUTE_TABLE_FULL;
  }
  Row* row_to_insert = &(statement->row_to_insert);

  uint32_t key_to_insert = row_to_insert->id;
  Cursor* cursor = table_find(table, key_to_insert);
  if (cursor->cell_num < num_cells)
  {
    uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
    if (key_at_index == key_to_insert)
    {
      return EXECUTE_DUPLICATE_KEY;
    }
  }

  leaf_node_insert(cursor, row_to_insert->id, row_to_insert);
  free(cursor);

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table)
{
  Row row;
  Cursor* cursor = table_start(table);
  while(!(cursor->end_of_table))
  {
    deserialize_row( cursor_value(cursor), &row);
    cursor_advance(cursor);
    print_row(&row);
  }

  free(cursor);
  return EXECUTE_SUCCESS;
}

#endif
