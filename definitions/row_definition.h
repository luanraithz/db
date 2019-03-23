#ifndef ROW_DEFINITION_H

#include <stdint.h>

#define ROW_DEFINITION_H

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
	uint32_t id;
	char username[COLUMN_USERNAME_SIZE + 1];
	char email[COLUMN_EMAIL_SIZE + 1];
} Row;

/*   Not sure if it's ugly or very ugly to do it,
 * just using this code to remember that it's possible to it.
 */
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define ID_SIZE size_of_attribute(Row, id)
#define USERNAME_SIZE size_of_attribute(Row, username)
#define EMAIL_SIZE size_of_attribute(Row, email)
#define ID_OFFSET (0)
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)
#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)

// Serializers

/* 	Short description of what the heck is `memcpy`
 *  
 * 	void *memcpy(void *str1, const void *str2, size_t n)
 *  
 *  Copies n characters from memory area str to memory area str1.
 * 	More info in:
 *    https://www.tutorialspoint.com/c_standard_library/c_function_memcpy.htm
 */

void serialize_row(Row* source, void* destination)
{
	memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
	memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
	memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void* source, Row* destination)
{
	memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
	memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
	memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void print_row(Row* row)
{
	printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

#endif