#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "input_functions.h"
#include "meta_commands.h"
#include "virtual_machine.h"
#include "definitions/row_definition.h"
#include "definitions/table_definition.h"

int main(int argc, char* argv[])
{
    Table* table = db_open(argc < 2 ? "defaultdb.db" : argv[1]);

    InputBuffer* input_buffer = new_input_buffer();

    while(true)
    {
        print_prompt();
        read_input(input_buffer);
        if (is_meta_command(input_buffer))
        {
            // `do_meta_command` may run the function `exit` if the input_buffer is ".exit"
            switch(do_meta_command(input_buffer, table))
            {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                    continue;
            }
        }

        Statement statement;
        StatementResult statement_result = prepare_statement(input_buffer, &statement);
        switch (statement_result.prepare_result)
        {
            case (PREPARE_SUCCESS):
                break;
            case (PREPARE_SYNTAX_ERROR):
                if (statement_result.message != NULL) {
                    printf("%s", statement_result.message);
                } else {
                    printf("Couldn't execute statement: '%s'.\n", input_buffer->buffer);
                }
                continue;
            case (PREPARE_UNRECOGNIZED_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer);
                continue;
        }

        switch (execute_statement(&statement, table))
        {
            case (EXECUTE_SUCCESS):
                printf("Executed.\n");
                break;
            case (EXECUTE_TABLE_FULL):
                printf("Error: Table full.\n");
                break;
        }
    }
}
