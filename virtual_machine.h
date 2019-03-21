#ifndef VIRTUAL_MACHINE_H

#define VIRTUAL_MACHINE_H

#include "statements/statements.h"
#include "definitions/row_definition.h"
#include "definitions/table_definition.h"

typedef enum {
	EXECUTE_SUCCESS,
	EXECUTE_TABLE_FULL
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
	if (table->num_rows >= TABLE_MAX_ROWS)
	{
		return EXECUTE_TABLE_FULL;
	}
	Row* row_to_insert = &(statement->row_to_insert);

	Cursor* cursor = table_end(table);
	serialize_row(row_to_insert, cursor_value(cursor));
	table->num_rows += 1;

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
