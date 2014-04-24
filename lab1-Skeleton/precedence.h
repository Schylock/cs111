#ifndef PRECEDENCE_H
#define PRECEDENCE_H
#include "stack_struct.h"
#include "command-internals.h"

typedef struct precedence
{
	int executed;  //flag: if command executed already   0:not run 1:run not returned, 2:returned
	int num_deps;  //number of dependencies, can executee if 0
	struct command* command;  //this command, for convenience
	struct stack_struct* dependees;   //this stores precidence_t's not command_t, since theyre the same size(pointer) its not an issue
							//points to all precidence objs that depend on the current precidence obj				
	//the following four were used only in getting the dependencies set up
	//you dont have to worry about them unless u really want to 
	int in_size;   //sizeof inputs
	int out_size;  //sizeof outputs
	char** inputs;   //all inputs to this command
	char** outputs;   //all outputs to this command
}precedence;

typedef precedence* precedence_t;

void printInputPrecedence(int* j, int* i, stack_type commands,precedence_t* prec, char* null,int* in_size,int* out_size );
void printOutputPrecedence(int* j, int* i, stack_type commands,precedence_t* prec, char* null,int* in_size,int* out_size );

#endif