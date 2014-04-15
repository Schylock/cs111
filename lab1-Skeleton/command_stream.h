#ifndef COMMAND_STREAM_H
#define COMMAND_STREAM_H

#include "alloc.h"
#include "command-internals.h"

typedef struct command_stream
{
	struct command_stream * next;
	char* word;
	int word_size;
	enum command_type type;
	int lineNumber;
}command_stream;

char* append(char* arr, int* size, char arg);
char** append2(char** arr, int* size, char* arg);
void print_stream(command_stream* stream);
command_stream* add_and_advance(command_stream* stream, char* buffer, int size, enum command_type type, int lineNumber);
void print(char** arr, int size);

#endif