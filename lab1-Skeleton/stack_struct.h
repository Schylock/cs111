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

typedef stack_struct* stack_type;

stack_type new_stack();
void push(stack_type stack, command* arg);
command* pop(stack_type stack);
command* top(stack_type stack);
void fix_precidence(stack_type expr_stack, stack_type op_stack);
void remove_elem(stack_type, command* );

#endif