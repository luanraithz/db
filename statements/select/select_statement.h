#ifndef SELECT_STATEMENT_H

#define SELECT_STATEMENT_H

#include "../statements.h"

typedef enum {
	SELECT_VALIDATION_SUCCESS
} SelectValidationStatus;

typedef struct {
	SelectValidationStatus validation_status;
	char* message;
} SelectValidationResult;

StatementResult select_statement(InputBuffer *input_buffer, Statement *statement)
{
	statement->type = STATEMENT_SELECT;
	StatementResult statement_result = { .prepare_result = PREPARE_SUCCESS, .message = NULL };
	return statement_result;
}

#endif