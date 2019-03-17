#ifndef STATEMENTS_H

#define STATEMENTS_H

#include <string.h>
#include "../input_functions.h"
#include "../definitions/row_definition.h"

typedef enum {
	PREPARE_SUCCESS,
	PREPARE_SYNTAX_ERROR,
	PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT
} StatementType;

typedef struct {
	StatementType type;
	Row row_to_insert; // Only used with type == STATEMENT_INSERT
} Statement;

typedef struct {
	PrepareResult prepare_result;
	char* message;
} StatementResult;

#include "insert/insert_statement.h"
#include "select/select_statement.h"

StatementResult prepare_statement(InputBuffer *input_buffer, Statement* statement)
{
	if (strncmp(input_buffer->buffer, "insert", 6) == 0)
	{
		return insert_statement(input_buffer, statement);
	}
	else if(strcmp(input_buffer->buffer, "select") == 0)
	{
		
		return select_statement(input_buffer, statement);
	}
	StatementResult statement_result = {
		.prepare_result = PREPARE_UNRECOGNIZED_STATEMENT,
		.message = NULL
	};
	return statement_result;
}

#endif
