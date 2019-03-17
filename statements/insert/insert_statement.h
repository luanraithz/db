#ifndef INSERT_STATEMENT_H

#define INSERT_STATEMENT_H

#include "../statements.h"
#include "../../input_functions.h"
#include "insert_validation.h"

StatementResult insert_statement(InputBuffer *input_buffer, Statement *statement)
{
	statement->type = STATEMENT_INSERT;

	InsertValidationResult validationResult = insert_validation(input_buffer, statement);
	if (validationResult.validation_status == INSERT_VALIDATION_SYNTAX_ERROR ) {
		StatementResult statement_result = {
			.prepare_result = PREPARE_SYNTAX_ERROR,
			.message = validationResult.message
		};
		return statement_result;
	} else {
		StatementResult statement_result = {
			.prepare_result = PREPARE_SUCCESS,
			.message = NULL
		};
		return statement_result;
	};
}

#endif
