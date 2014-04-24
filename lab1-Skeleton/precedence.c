#include "precedence.h"
#include <string.h>
#include "command-internals.h"
#include "stack_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include "command.h"

void printInputPrecedence(int* j, int* i, stack_type commands,precedence_t* prec, char* null,int* in_size,int* out_size )
{
	for(*j =*i + 1; *j < commands->contained; (*j)++)
	{
		for (*in_size = 0; *in_size < prec[(*j)]->in_size; (*in_size)++)
		{
			if (strcmp(prec[(*i)]->outputs[*out_size], prec[(*j)]->inputs[*in_size]) == 0)
			{
				printf("%s, %s\n", prec[(*i)]->outputs[*out_size], prec[(*j)]->inputs[*in_size]);
				//push(prec[j]->dependencies, commands->items[i]);
				//push(prec[i]->dependees, commands->items[j]);
				push(prec[(*i)]->dependees, (command_t) prec[(*j)]);
				prec[(*j)]->num_deps++;
				break;
			}
		}
	}
}

void printOutputPrecedence(int* j, int* i, stack_type commands,precedence_t* prec, char* null,int* in_size,int* out_size )
{
	for(*j=*i + 1; *j < commands->contained; (*j)++)
	{
		for (*out_size = 0; *out_size < prec[(*j)]->out_size; (*out_size)++)
		{
			if (strcmp(prec[(*i)]->inputs[*in_size], prec[(*j)]->outputs[*out_size]) == 0)
			{
				printf("%s, %s\n", prec[(*i)]->inputs[*in_size], prec[(*j)]->outputs[*out_size]);
				//push(prec[j]->dependencies, commands->items[i]);
				//push(prec[i]->dependees, commands->items[j]);
				push(prec[(*i)]->dependees, (command_t) prec[(*j)]);
				prec[(*j)]->num_deps++;
				break;
			}
		}
	}
}