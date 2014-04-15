#include "command.h"
#include "command_stream.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
char * append(char* arr, int* size, char arg)
{
  char * res;
  if (*size == 0)
  {
	res = (char*)checked_malloc((*size));
	res[0] = arg;
	(*size)++;
	return (char*)res;
  }
  res = (char*)checked_realloc(arr, (*size) + 1);
  res[(*size)] = arg;
  (*size)++;
  return (char*)res;
}

char** append2(char** arr, int* size, char* arg)
{
  char ** res;
  if (*size == 0)
  {
    res = (char**)checked_malloc( sizeof(char*) );
  }
  else
  {
    res = (char**)checked_realloc(arr, ((*size)+ 1) * sizeof(char*));
  }
  res[(*size)] = arg;
  (*size)++;
  return res;
} 

void print_stream(command_stream_t stream)
{
  stream = stream -> next;
  while( stream != NULL)
  {
    //printf("Token: %s, Size: %d, Type: %d\n", stream-> word, stream-> word_size, 
		//stream -> type);
    if (stream-> next == NULL || stream->next->word_size == 0)
	{
	  stream-> next = NULL;
	}
	stream = stream-> next;
  }

}

command_stream* add_and_advance(command_stream_t stream, char* buffer, int size, enum command_type type, int lineNumber)
{
  buffer = append(buffer, &size, '\0');
  size--;
  stream-> word = buffer;
  stream-> word_size = size;
  stream-> type = type;
  stream-> next = checked_malloc(sizeof(struct command_stream));
  stream-> lineNumber = lineNumber;
  return stream->next;
 }
 
  void print(char** arr, int size)
 {
  int c = 0;
  while (c < size)
  {
    printf("%s\n", arr[c]);
    c++;
  }
  
} 