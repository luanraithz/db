#ifndef META_COMMANDS_H

#define META_COMMANDS_H

#include "input_functions.h"

int is_meta_command(InputBuffer* buffer);

typedef enum {
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

/*
	Meta commands are the ones that start with an ".", like: ".exit"
*/
MetaCommandResult do_meta_command(InputBuffer* input_buffer)
{
	if (strcmp(input_buffer->buffer, ".exit") == 0)
	{
		exit(EXIT_SUCCESS);
	} else {
		return META_COMMAND_UNRECOGNIZED_COMMAND;
	}
}

int is_meta_command(InputBuffer* input_buffer) {
	return input_buffer->buffer[0] == '.';
}

#endif
