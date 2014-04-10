// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include "alloc.h"
#include <string.h>
#include <error.h>
#include <stdlib.h>
#include <stdio.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

enum state_enum
{
/*  in,. out is put in simple */ 
  PIPE,
  AND,
  CHAR,
  SPACE,
};
  
struct command_stream
{
	struct command_stream * next;
	char* word;
	int word_size;
	enum command_type type;
};

struct stack_struct
{
  int contained;
  int items_size;
  command_t * items;
};

typedef struct stack_struct * stack_t;
 
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
 
command_stream_t add_and_advance(command_stream_t stream, char* buffer, int size, enum command_type type)
{
  buffer = append(buffer, &size, '\0');
  size--;
  stream-> word = buffer;
  stream-> word_size = size;
  stream-> type = type;
  stream-> next = checked_malloc(sizeof(struct command_stream));
  return stream->next;
}
 
   
command_stream_t make_command_stream (int (*get_next_byte) (void *), void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  enum state_enum state = SPACE;
  //states = { \0, whitespace, |, &
  int c;
  char * buffer = NULL;
  int buffer_size = 0;
  int paren_count = 0;
  int lineNumber=1;
  command_stream_t stream_head = checked_malloc(sizeof(struct command_stream));
  stream_head -> next = checked_malloc(sizeof(struct command_stream));
  command_stream_t stream = stream_head-> next;
  while ( (c = get_next_byte(get_next_byte_argument)) != EOF)
  {
    //printf("%d\n", state);
	if (paren_count < 0)
	{
	  fprintf(stderr, "%d: Too many closing parens\n", lineNumber);
	}
    switch(c)
	{
	  case ';':
	    if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = 'c';
		  stream = add_and_advance(stream, tmp, 1, SEQUENCE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = ';';
		  stream = add_and_advance(stream, tmp, 1, SEQUENCE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		break;
	  case '>' :
	    if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '>';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '>';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		break;
	  case '<' :
	    if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '<';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '<';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		break;
	  case '#':
		while( (c = get_next_byte(get_next_byte_argument)) != '\n')
		{
		  if (c == EOF) break;
		}
		break;
		
	  case '|':
		if (state == PIPE)
		{
		  char *tmp = checked_malloc(sizeof(char)*2);
		  tmp[0] = '|';
		  tmp[1] = '|';
		  stream = add_and_advance(stream, tmp, 2, OR_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		  state = PIPE;
		}
		else if (state != PIPE)
		{
		  state = PIPE;
		}
		break;
	  case '&':
		if (state == AND)
		{
		  char *tmp = checked_malloc(sizeof(char)*2);
		  tmp[0] = '&';
		  tmp[1] = '&';
		  stream = add_and_advance(stream, tmp, 2, AND_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		} 
		else if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		  state = AND;
		}
		else if (state == SPACE)
		{
		  state = AND;
		}
		break;
	  case '(':
		if (state == PIPE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '|';
		  stream = add_and_advance(stream, tmp, 1, PIPE_COMMAND);
		  tmp = checked_malloc(sizeof(char));
		  tmp[0] = '(';
		  stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND);
		  paren_count++;
		  state = SPACE;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '(';
		  stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND);
		  paren_count++;
		  state = SPACE;
		}
		break;
	   case ')':
	     if (state == CHAR)
		 {
		   stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
		   state = SPACE;
		   buffer_size = 0;
		   char *tmp = checked_malloc(sizeof(char));
		   tmp[0] = ')';
		   stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND);
		   paren_count--;
		 }
		 else if(state == SPACE)
		 {
		   char *tmp = checked_malloc(sizeof(char));
		   tmp[0] = ')';
		   stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND);
		   paren_count--;
		 
		 }
	  
	  case '\n':
	   lineNumber++;
		//fall thru
	  case '\t':
		//fall thru  */
		
	  case 13:
	  case ' ':
	    if (state == PIPE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '|';
		  stream = add_and_advance(stream, tmp, 1, PIPE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		  if (c == '\n')
		  {
		    char * tmp = checked_malloc(sizeof(char));
			tmp[0] = '\n';
			stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND);
		  }
		}
		else if (state == SPACE){
		  if (c == '\n')
		  {
		    char * tmp = checked_malloc(sizeof(char));
			tmp[0] = '\n';
			stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND);
		  }	
		  continue;
		}
		break;
	  default:
		if (state == CHAR)
		{
		  buffer = append(buffer, &buffer_size, c);
		}
		else if(state == PIPE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '|';
		  stream = add_and_advance(stream, tmp, 1, PIPE_COMMAND);
		  state = SPACE;
		  buffer_size = 0;
		  buffer = append(buffer, &buffer_size, c);
		  state = CHAR;
		}
		else if(state != SPACE)
		{
		  fprintf(stderr, "%d: Dangling Operator\n", lineNumber);
		  //error (1, 0, "Dangling Operator");
		}
		else 
		{
		  if ( !((c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == '!' || c == '%' || c == '+'
				|| c == '-' || c == '.' || c == '/' || c == ':' || c == '@' || c == '^' || c== '_'))
		  {
			 fprintf(stderr, "%d: Command beginning with containing letter\n", lineNumber);
		    //error (1, 0, "Command beginning with non letter");
		  }
		  buffer = append(buffer, &buffer_size, c);
		  state = CHAR;
		}
	  }
	}
  //if file doesnt end on white space , wir haben problem
  //error (1, 0, "command reading not yet implemented");
  if(paren_count != 0)
  {
    fprintf(stderr, "%d: Open paren not matched\n", lineNumber);
    //error (1, 0, "Open paren not matched");
  }
  if (buffer_size != 0)
  {
    add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND);
  }
  stream-> next = NULL;
  print_stream(stream_head);
  return stream_head;
}

stack_t new_stack()
{
  stack_t res = checked_malloc(sizeof(struct stack_struct));
  res-> items = checked_malloc(sizeof(command_t)*64);
  res-> items_size = 64;
  res-> contained = 0;
  return res;
}

void push(stack_t stack, command_t arg)
{
  if( stack-> contained == stack -> items_size)
  {
	stack-> items = checked_realloc(stack, sizeof(struct stack_struct)*stack->items_size * 2);
	stack-> items_size *= 2;
  }
  stack-> items[stack->contained] = arg;
  stack-> contained++;
}

command_t pop(stack_t stack)
{
  stack-> contained--;
  return stack-> items[stack->contained];
}

command_t top(stack_t stack)
{
  return stack-> items[stack->contained - 1];
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

void fix_precidence(stack_t expr_stack, stack_t op_stack)
{
  command_t command = pop(op_stack);
  command -> u.command[1] = pop(expr_stack);
  command -> u.command[0] = pop(expr_stack);
  push(expr_stack, command);
}

command_t read_command_stream (command_stream_t stream)
{
  int prev_is_simple = 0, buffer_size = 0;
  stack_t expr_stack = new_stack(), op_stack = new_stack();
  char * null = NULL;
  command_stream_t s = stream-> next, prev_s;
  command_t command  = NULL;
  char ** buffer;
  if (s == NULL)
	return NULL;
  while (s != NULL)
  {
   switch(s ->type)
   {
    case AND_COMMAND:
		command = checked_malloc(sizeof(struct command));
		command -> type = AND_COMMAND;
		while(op_stack -> contained > 0 )
		{
		  fix_precidence(expr_stack, op_stack);
		}
		prev_is_simple = 0;
		push(op_stack, command);
		break;
	case OR_COMMAND:
		command = checked_malloc(sizeof(struct command));
		command -> type = OR_COMMAND;
		while(op_stack -> contained > 0 )
		{
		  fix_precidence(expr_stack, op_stack);
		}
		prev_is_simple = 0;
		push(op_stack, command);
		break;
	case PIPE_COMMAND:
		command = checked_malloc(sizeof(struct command));
		command -> type = PIPE_COMMAND;
		while (op_stack-> contained > 0 && top(op_stack) -> type == PIPE_COMMAND)
		{
		  fix_precidence(expr_stack, op_stack);
		}
		push(op_stack, command);
		prev_is_simple = 0;
		break;
	case SEQUENCE_COMMAND: 
		command = checked_malloc(sizeof(struct command));
		command -> type = SEQUENCE_COMMAND;
		while(op_stack -> contained > 0 )
		{
		  fix_precidence(expr_stack, op_stack);
		}
		prev_is_simple = 0;
		push(op_stack, command);
		break;
	case SUBSHELL_COMMAND:
		if (s-> word[0] == '(')
		{
			command = checked_malloc(sizeof(struct command));
			command -> type = SUBSHELL_COMMAND;
			push(op_stack, command);
		}
		else if (s-> word[0] == ')')
		{
		  prev_is_simple = 0;
		  while((command = pop(op_stack))-> type != SUBSHELL_COMMAND)
		  {
		    command->u.command[1] = pop(expr_stack);
	        command->u.command[0] = pop(expr_stack);
			push(expr_stack, command);
		  }
		  command-> u.subshell_command = pop(expr_stack);
		  push(expr_stack, command);
		}
		break;
	default:
	  if ( prev_is_simple )
	  {
	    error (1, 0, "command reading not yet implemented");
	  }
	  command = checked_malloc(sizeof(struct command));
	  command -> type = SIMPLE_COMMAND;
	  push(expr_stack, command); 
	  while( s && s-> word[0] == '\n')
		s = s-> next;
	  while( s && s-> type == SIMPLE_COMMAND)
	  {
	    if ( (s-> word[0] == '<' || s-> word[0] == '>') && s-> next == NULL)
		{
          error (1, 0, "Redirection error");
		}
	    if (s-> word[0] == '<' && s-> next-> type == SIMPLE_COMMAND)
		{
		  if (buffer_size == 0  || s-> next -> word[0] == '>' || s-> next-> word[0] == '<' || s->next == NULL)
		  {
		    error (1, 0, "Redirection error");
		  }
		  command-> input = s-> next-> word;
		  prev_s = s -> next;
		  s = s-> next -> next;
		  continue;
		}
		if (s-> word[0] == '>' && s-> next-> type == SIMPLE_COMMAND)
		{
		  if (buffer_size == 0  || s-> next -> word[0] == '>' || s-> next-> word[0] == '<' || s->next == NULL)
		  {
		    error (1, 0, "Redirection error");
		  }
		  command-> output = s-> next-> word;
		  prev_s = s -> next;
	      s = s-> next -> next;
		  continue;
		}
		while (s && s-> word[0] == '\n')
		{
		  s = s-> next;
		  prev_is_simple = -1;
		}
		if (prev_is_simple == -1){
			buffer = append2(buffer, &buffer_size, null);
			command->u.word = buffer;
			goto end;
		}
	    buffer = append2(buffer, &buffer_size, s->word);
		
		prev_s = s;
		s = s-> next;
	  }
	  command -> u.word = buffer;
	  buffer = append2(buffer, &buffer_size, null);
	  s = prev_s;
	  buffer_size = 0;
      prev_is_simple = 1; 
	}
	s = s-> next;
  }
  end:
  if (buffer_size > 0 )
  {
	buffer = append2(buffer, &buffer_size, null);
	command->u.word = buffer;
  }
  
  while (op_stack->contained != 0)
  {
    if (expr_stack-> contained < 2)
	{
	  error (1, 0, "Invalid syntax");
	}
    fix_precidence(expr_stack, op_stack);
  }
    
  /* FIXME: Replace this with your implementation too.  */
  //
  stream-> next = s;
  if (expr_stack-> contained == 1)
  {
    return top(expr_stack);
  }
  else if(expr_stack-> contained != 0)
  {
    error (1, 0, "command reading not yet implemented");
  }
  return NULL;
}
