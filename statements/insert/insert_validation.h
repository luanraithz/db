#ifndef INSERT_VALIDATOR_H

#define INSERT_VALIDATOR_H

#include "../../input_functions.h"
#include "../../definitions/row_definition.h"

typedef enum {
	INSERT_VALIDATION_SUCCESS,
	INSERT_VALIDATION_SYNTAX_ERROR
} InsertValidationStatus;

typedef struct {
	InsertValidationStatus validation_status;
	char* message;
} InsertValidationResult;

typedef enum {
	QUERY_VALID,
	QUERY_FIELD_TO_LONG,
	QUERY_SYNTAX_ERROR,
	QUERY_INVALID_ID
} QuerySyntaxStatus;

QuerySyntaxStatus validate_query(InputBuffer *input_buffer, Statement* statement);

InsertValidationResult insert_validation(InputBuffer* input_buffer, Statement *statement) {
	
	QuerySyntaxStatus syntax_status = validate_query(input_buffer, statement);
	switch (syntax_status)
	{
		case (QUERY_FIELD_TO_LONG):
			return (InsertValidationResult){ .validation_status = INSERT_VALIDATION_SYNTAX_ERROR, .message = "One of the fields is too large.\n" };
		case (QUERY_SYNTAX_ERROR):
			return (InsertValidationResult){ .validation_status = INSERT_VALIDATION_SYNTAX_ERROR, .message = "Invalid insert statement.\n" };
		case (QUERY_INVALID_ID):
			return (InsertValidationResult){ .validation_status = INSERT_VALIDATION_SYNTAX_ERROR, .message = "Invalid id.\n" };
		default:
			return (InsertValidationResult){ .validation_status = INSERT_VALIDATION_SUCCESS, .message = NULL };
	};
	
};

QuerySyntaxStatus validate_query(InputBuffer *input_buffer, Statement* statement)
{
	char *keyword = strtok(input_buffer->buffer, " ");
	char *id_string = strtok(NULL, " ");
	char *username = strtok(NULL, " ");
	char *email = strtok(NULL, " ");
	if (id_string == NULL || username == NULL || email == NULL)
	{
		return QUERY_SYNTAX_ERROR;
	}

	int id = atoi(id_string);
	if (id < 0) {
		return QUERY_INVALID_ID;
	}
	if (strlen(username) > COLUMN_USERNAME_SIZE || strlen(email) > COLUMN_USERNAME_SIZE)
	{
		return QUERY_FIELD_TO_LONG;
	}
	statement->row_to_insert.id = id;
	strcpy(statement->row_to_insert.username, username);
	strcpy(statement->row_to_insert.email, email);


	return QUERY_VALID;
}

#endif