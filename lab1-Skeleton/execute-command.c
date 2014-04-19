// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <stdlib.h>
#include <error.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int command_status (command_t c)
{
  return c->status;
}

void runSimpleCommand(command_t c)
{
	pid_t pid=fork();
	if(pid < 0) error(1, 0, "A fork has failed us");
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
			outputFD=open(c->output,O_RDWR);
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
	if(pid[0] < 0) error(1, 0, "A fork has failed us");
	if(pid[0]==0)
	{
		pid[1]=fork();
		if(pid[1] < 0) error(1, 0, "A fork has failed us");
		if(pid[1]==0)
		{
			close(fd[1]);
			dup2(fd[0],0);
			close(fd[0]);
			status=execute_command(c->u.command[1],0);
			status = (status & 0xff00) >> 8;
			exit(c->status);
		}
		else
		{
			close(fd[0]);
			dup2(fd[1],1);
			close(fd[1]);
			status = execute_command(c->u.command[0],0);
			status = (status & 0xff00) >> 8;
			exit(c->status);
		}
	}
	else
	waitpid(pid[0],&(c->status),0);
}

int execute_command (command_t c, int time_travel)
{
	pid_t pid;
	switch (c->type)
    {
		case AND_COMMAND:
		c->status=execute_command(c->u.command[0],0);
		if(c->status==0)
			c->status=execute_command(c->u.command[1],0);
		break;
		case OR_COMMAND:
		c->status=execute_command(c->u.command[0],0);
		if(c->status)
			c->status=execute_command(c->u.command[1],0);
		break;
		case SEQUENCE_COMMAND:
		c->status=execute_command(c->u.command[0],0);
		c->status=execute_command(c->u.command[1],0);
		break;
		case PIPE_COMMAND:
		runPipeCommand(c);
		break;
		case SIMPLE_COMMAND:
		runSimpleCommand(c);
		break;
		case SUBSHELL_COMMAND:
		c->status=execute_command(c->u.subshell_command,0);
		break;
		default:
		abort ();
	}

	//printf("Return status is %d\n", c->status);
	return c->status;
}
