// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include "command_stream.h"
#include "stack_struct.h"
#include <stdlib.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include "precedence.h"
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

   
int exec_seq(command_t );
stack_type commands;
   
int command_status (command_t c)
{
  return c->status;
}

void runSimpleCommand(command_t c)
{
	pid_t pid=fork();
	if(pid < 0) error(1, pid, "A fork has failed us");
	if(pid==0)
	{
		int inputFD;//if input, change to input else change to stdin
		if(c->input)
		{
			inputFD=open(c->input,O_RDWR);
			close(0);
			dup(inputFD);
			close(inputFD);
		}
		int outputFD;//if output
		if(c->output)
		{
			outputFD=open(c->output,O_RDWR | O_CREAT, S_IRWXU);
			close(1);
			dup(outputFD);
			close(outputFD);
		}
		c->status=execvp(c->u.word[0],c->u.word);
		c->status = (c->status & 0xff00) >> 8;
	}
	else
	while(pid!=wait(&(c->status)))
	{
		continue;
	} 
}

void runPipeCommand(command_t c)
{
	int fd[2], status;
	pid_t pid[2];
	pipe(fd);
	pid[0]=fork();
	if(pid[0] < 0) error(1, pid[0], "A fork has failed us");
	if(pid[0]==0)
	{
		pid[1]=fork();
		if(pid[1] < 0) error(1, pid[1], "A fork has failed us");
		if(pid[1]==0)
		{
			close(fd[1]);
			dup2(fd[0],0);
			close(fd[0]);
			status=exec_seq(c->u.command[1]);
			status = (status & 0xff00) >> 8;
			exit(c->status);
		}
		else
		{
			close(fd[0]);
			dup2(fd[1],1);
			close(fd[1]);
			status = exec_seq(c->u.command[0]);
			status = (status & 0xff00) >> 8;
			exit(c->status);
		}
	}
	else
	waitpid(pid[0],&(c->status),0);
}

int exec_seq(command_t c)
{
	pid_t pid;
	switch (c->type)
    {
		case AND_COMMAND:
		c->status=exec_seq(c->u.command[0]);
		if(c->status==0)
			c->status=exec_seq(c->u.command[1]);
		break;
		case OR_COMMAND:
		c->status=exec_seq(c->u.command[0]);
		if(c->status)
			c->status=exec_seq(c->u.command[1]);
		break;
		case SEQUENCE_COMMAND:
		c->status=exec_seq(c->u.command[0]);
		c->status=exec_seq(c->u.command[1]);
		break;
		case PIPE_COMMAND:
		runPipeCommand(c);
		break;
		case SIMPLE_COMMAND:
		runSimpleCommand(c);
		break;
		case SUBSHELL_COMMAND:
		c->status=exec_seq(c->u.subshell_command);
		break;
		default:
		abort ();
	}

	//printf("Return status is %d\n", c->status);
	return c->status;


}


void print_stuff(precedence_t* prec)
{
	int i, j;
	char ** w;
	printf("\nprec fields:\n");
	for (i = 0; i < commands->contained; i++)
	{
		printf("inputs: \n");
		for (j = 0; j < prec[i]->in_size; j++)
			printf("%s\n", prec[i]->inputs[j]);
		printf("outputs: \n");
		for (j = 0; j < prec[i]->out_size; j++)
			printf("%s\n", prec[i]->outputs[j]);
		printf("%d dependees\n", prec[i]->dependees->contained);
	}
}

//should eventually execute all the commands in parellel
//right now it just prints stuff for a sanity check

int exec_parellel(precedence_t* prec)
{
	//print_stuff(prec);
	int i=0;
	int inner=0;
	int pid;
	int numdep;
	int status;
	precedence_t ptr;
	while(i<commands->contained)
	{
		for(inner=0;inner<commands->contained;inner++)
		{
			if(prec[inner]->num_deps>0)
				continue;
			if(prec[inner]->executed==0)
			{
				pid=fork();
				if(pid==0)
				{
					exit(exec_seq(prec[inner]->command));
				}
				else if(pid<0)
					error(1,0,"%d is not a valid thread number",pid);
				else
				{
					prec[inner]->pid=pid;
					prec[inner]->executed = 1;
				}	
				
			}
			
		}
		pid=wait(&status);
		i++;
		for(inner=0;inner<commands->contained;inner++)
		{
			if(prec[inner]->pid==pid)
			{
				for(numdep=0;numdep<prec[inner]->dependees->contained;numdep++)
				{
					ptr=(precedence_t)(prec[inner]->dependees->items[numdep]);
					(ptr->num_deps)--;
				}
				break;
			}
		}
	}
	return commands->items[commands->contained - 1]->status;
}

void get_redirs(precedence_t prec, int* in_size, int* out_size, command_t c)
{
	switch (c->type)
	{
		case AND_COMMAND:
		case OR_COMMAND:
		case PIPE_COMMAND:
		case SEQUENCE_COMMAND:
			get_redirs(prec, in_size, out_size, c->u.command[0]);
			get_redirs(prec, in_size, out_size, c->u.command[1]); 
			break;
		case SUBSHELL_COMMAND:
			get_redirs(prec, in_size, out_size, c->u.subshell_command);
			break;
		case SIMPLE_COMMAND:
			if (c->input)
				prec->inputs = append2(prec->inputs, in_size, c->input); 
			if (c->output)
				prec->outputs = append2(prec->outputs, out_size, c->output);
			char **w = c->u.word;
			while(w && *++w)
				prec->inputs = append2(prec->inputs, in_size, *w);
			break;
		default:
			exit(-1);
	}

}

precedence_t*  establish_precedence()
{
//setups
	int i, j;
	int * in_size = checked_malloc(sizeof(int));
	int * out_size = checked_malloc(sizeof(int));
	char * null = NULL;
	precedence_t*  prec = checked_malloc(sizeof(precedence_t)* commands->contained);
//getting all the inputs and output strings	
	for(i = 0; i < commands->contained; i++)
	{
		*in_size = 0;
		*out_size = 0;
		prec[i] = checked_malloc(sizeof(precedence));
		get_redirs(prec[i], in_size, out_size, commands->items[i]);
		prec[i]->in_size = *in_size;
		prec[i]->out_size = *out_size;
		prec[i]->num_deps = 0;
		prec[i]->inputs = append2(prec[i]->inputs, in_size, null);
		prec[i]->outputs = append2(prec[i]->outputs, out_size, null);
		prec[i]->command = commands->items[i];
		prec[i]->executed = 0;	
		prec[i]->dependees = new_stack();
	}
/* a later process depends on an earlier process if: 
	its output is the input of earlier process
	its ooutput is the output of earlier process
	its input is output of earlier process */
	//printf("List of all matched dependencies:\n");
	for(i = 0; i < commands->contained - 1; i++)
	{
		for( *in_size = 0; *in_size < prec[i]->in_size; (*in_size)++)
		{
			printOutputPrecedence(&j,&i,commands,prec,null,in_size,out_size );
		}
		for( *out_size = 0; *out_size < prec[i]->out_size; (*out_size)++)
		{
			printInputPrecedence(&j,&i,commands,prec,null,in_size,out_size );
			printInputPrecedence(&j,&i,commands,prec,null,in_size,out_size );
		}
		
	}
	return prec;
} 


int execute_command (command_t c, int time_travel)
{
	if (!time_travel)
		return exec_seq(c);
	else {
		if (commands == NULL)
			commands = new_stack();
		if (c == NULL)
		{
			precedence_t*  prec = establish_precedence();
			return exec_parellel(prec);
		}
		push(commands, c);
	}
	return 0;
}

