#include "stack_struct.h"
#include "command.h"

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

void fix_precidence(stack_t expr_stack, stack_t op_stack)
{
  command_t command = pop(op_stack);
  command -> u.command[1] = pop(expr_stack);
  command -> u.command[0] = pop(expr_stack);
  push(expr_stack, command);
}
