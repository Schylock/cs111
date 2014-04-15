#ifndef STACK_STRUCT_H
#define STACK_STRUCT_H

#include "alloc.h"
#include "command-internals.h"

typedef struct stack_struct
{
  int contained;
  int items_size;
  command** items;
}stack_struct;

typedef stack_struct* stack_t;

stack_t new_stack();
void push(stack_t stack, command* arg);
command* pop(stack_t stack);
command* top(stack_t stack);
void fix_precidence(stack_t expr_stack, stack_t op_stack);

#endif