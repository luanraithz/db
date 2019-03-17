#ifndef VIRTUAL_MACHINE_H

#define VIRTUAL_MACHINE_H

#include "statements.h"

void execute_statement(Statement* statement)
{
	switch (statement->type)
	{
		case (STATEMENT_INSERT):
			printf("This is where we would do an insert. \n");
			printf("%s \n", statement->row_to_insert.email);
			break;
		case (STATEMENT_SELECT):
			printf("This is where we would do a select \n");
			break;
	}
	
}
#endif
