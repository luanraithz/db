#ifndef STATEMENTS_H

#define STATEMENTS_H

#include <string.h>
#include "input_functions.h"

typedef enum {
	PREPARE_SUCCESS,
	PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT
} StatementType;

typedef struct {
	StatementType type;
} Statement;

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement* statement)
{
	if (strncmp(input_buffer->buffer, "insert", 6) == 0)
	{
		statement->type = STATEMENT_INSERT;
		return PREPARE_SUCCESS;
	} else if(strcmp(input_buffer->buffer, "select") == 0)
	{
		statement->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}
	return PREPARE_UNRECOGNIZED_STATEMENT;
}

#endif
