// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command_stream.h"
#include "stack_struct.h"
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
	  error (1, 0, "%d: Too many closing parens\n", lineNumber);
	}
    switch(c){
	  case ';':
	    if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = 'c';
		  stream = add_and_advance(stream, tmp, 1, SEQUENCE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = ';';
		  stream = add_and_advance(stream, tmp, 1, SEQUENCE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		}
		break;
	  case '>' :
	    if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '>';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '>';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		}
		break;
	  case '<' :
	    if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '<';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '<';
		  stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND, lineNumber);
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
		  stream = add_and_advance(stream, tmp, 2, OR_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
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
		if (state == AND){
		  char *tmp = checked_malloc(sizeof(char)*2);
		  tmp[0] = '&';
		  tmp[1] = '&';
		  stream = add_and_advance(stream, tmp, 2, AND_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		} 
		else if (state == CHAR){
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		  state = AND;
		}
		else if (state == SPACE){
		  state = AND;
		}
		break;
	  case '(':
		if (state == PIPE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '|';
		  stream = add_and_advance(stream, tmp, 1, PIPE_COMMAND, lineNumber);
		  tmp = checked_malloc(sizeof(char));
		  tmp[0] = '(';
		  stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND, lineNumber);
		  paren_count++;
		  state = SPACE;
		}
		else if (state == SPACE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '(';
		  stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND, lineNumber);
		  paren_count++;
		  state = SPACE;
		}
		break;
	   case ')':
	     if (state == CHAR)
		 {
		   stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
		   state = SPACE;
		   buffer_size = 0;
		   char *tmp = checked_malloc(sizeof(char));
		   tmp[0] = ')';
		   stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND, lineNumber);
		   paren_count--;
		 }
		 else if(state == SPACE)
		 {
		   char *tmp = checked_malloc(sizeof(char));
		   tmp[0] = ')';
		   stream = add_and_advance(stream, tmp, 1, SUBSHELL_COMMAND, lineNumber);
		   paren_count--;
		 
		 }
		 break;
	  case '\n':
	    lineNumber++; 
		//fall thru
	  case '\t':
		//fall thru  */
	  case ' ':
	    if (state == PIPE)
		{
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '|';
		  stream = add_and_advance(stream, tmp, 1, PIPE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		}
		else if (state == CHAR)
		{
		  stream = add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		  if (c == '\n'){
		    char * tmp = checked_malloc(sizeof(char));
			tmp[0] = '\n';
			stream = add_and_advance(stream, tmp, 1, SIMPLE_COMMAND, lineNumber);
		  }
		}
		else if (state == SPACE){
		  continue;
		}
		break;
	  default:
		if (state == CHAR){
		  buffer = append(buffer, &buffer_size, c);
		}
		else if(state == PIPE){
		  char *tmp = checked_malloc(sizeof(char));
		  tmp[0] = '|';
		  stream = add_and_advance(stream, tmp, 1, PIPE_COMMAND, lineNumber);
		  state = SPACE;
		  buffer_size = 0;
		  buffer = append(buffer, &buffer_size, c);
		  state = CHAR;
		}
		else if(state != SPACE)
		{
		  error (1, 0, "%d: Dangling Operator\n", lineNumber);
		}
		else 
		{
		  if ( !((c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == '!' || c == '%' || c == '+'
				|| c == '-' || c == '.' || c == '/' || c == ':' || c == '@' || c == '^' || c== '_'))
		  {
		    error (1, 0, "%d: Command with invalid letter\n", lineNumber);
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
    error (1, 0, "%d: Open paren not matched\n", lineNumber);
  }
  if (buffer_size != 0)
  {
    add_and_advance(stream, buffer, buffer_size, SIMPLE_COMMAND, lineNumber);
  }
  stream-> next = NULL;
  print_stream(stream_head);
  return stream_head;
}

command_t read_command_stream (command_stream_t stream)
{
  int prev_is_simple = 0, buffer_size = 0, lineNumber;
  stack_t expr_stack = new_stack(), op_stack = new_stack();
  char * null = NULL;
  command_stream_t s = stream-> next, prev_s;
  command_t command  = NULL;
  char ** buffer;
  while (s != NULL)
  {
   lineNumber = s->lineNumber;
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
	    error (1, 0, "%d: Consequtive Simple Commands.", s-> lineNumber);
	  }
	  command = checked_malloc(sizeof(struct command));
	  command -> type = SIMPLE_COMMAND;
	  push(expr_stack, command); 
	  while( s && s-> type == SIMPLE_COMMAND)
	  {
	    if ( (s-> word[0] == '<' || s-> word[0] == '>') && s-> next == NULL)
		{
          error (1, 0, "%d: Redirection error", s-> lineNumber);
		}
	    if (s-> word[0] == '<' && s-> next-> type == SIMPLE_COMMAND)
		{
		  if (buffer_size == 0  || s-> next -> word[0] == '>' || s-> next-> word[0] == '<' || s->next == NULL)
		  {
		    error (1, 0, "%d: Redirection error", s-> lineNumber);
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
		    error (1, 0, "%d: Redirection error", s-> lineNumber);
		  }
		  command-> output = s-> next-> word;
		  prev_s = s -> next;
	      s = s-> next -> next;
		  continue;
		}
		if (s-> word[0] == '\n')
		{
		  s = s-> next;
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
  if (buffer_size != 0)
  {
	buffer = append2(buffer, &buffer_size, null);
	command->u.word = buffer;
  }
  
  while (op_stack->contained != 0)
  {
    if (expr_stack-> contained < 2)
	{
	  error (1, 0, "%d: Invalid command syntax", lineNumber);
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
    error (1, 0, "%d: Invalid command syntax", lineNumber - 1);
  }
  return NULL;
}
