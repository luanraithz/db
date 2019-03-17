#ifndef META_COMMANDS_H

#define META_COMMANDS_H

#include "input_functions.h"

typedef enum {
	META_COMMAND_SUCCESS,
	META_COMMAND_UNRECOGNIZED_COMMAND
} MetdaCommandResult;

/*
	Meta commands are the ones that start with an ".", like: ".exit"
*/
MetdaCommandResult do_meta_command(InputBuffer* input_buffer)
{
	if (strcmp(input_buffer->buffer, ".exit") == 0)
	{
		exit(EXIT_SUCCESS);
	} else {
		return META_COMMAND_UNRECOGNIZED_COMMAND;
	}
}

#endif
