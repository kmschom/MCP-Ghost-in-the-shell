/*
 * part2.c
 *
 * Created on: Nov 3, 2021
 *     Author: Kelly Schombert
 *
 * Copied from part1 on Nov. 7
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "string_parser.h"

int main(int argc, char const *argv[])
{
	// Check for ./part1.c [FILENAME]
	if(argc != 2)
	{
		printf("Error: 1 input file required\n");
		return 1;
	}

	// Check that file exists
	FILE *inputFile = fopen(argv[1], "r");
	if(inputFile == NULL)
	{
		printf("Error: %s does not exist. Input name of 1 existing file.\n", argv[2]);
		return 1;
	}

	// Var setup
	char *line_buf = NULL;
	char *token = NULL;
	char *path = NULL;
	char *argList[10];
	
	size_t len = 0;
	int i = 0;
	int numLines = 0;
	int prog = 0;
	int k = 0;
	
	while(getline(&line_buf, &len, inputFile) != -1)
	{
		numLines++;
	}
	fclose(inputFile);

	inputFile = fopen(argv[1], "r");

	command_line large_token_buffer;
	
	pid_t* pid_array;
	pid_array=(pid_t*)malloc(sizeof(pid_t) * numLines);

	// Set signal handler for SIGUSR1
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	int sig;
	int result;
	

	// Reading input file of commands
	while(getline(&line_buf, &len, inputFile) != -1)
	{
		// Tokenize line buffer; large token separated by " "
		large_token_buffer = str_filler(line_buf, " ");

		path = large_token_buffer.command_list[0];

		for (int j = 0; large_token_buffer.command_list[j] != NULL; j++)
		{
			token = large_token_buffer.command_list[j];
			argList[j] = token;
			k++;
		}
		
		argList[k] = NULL;
	
		pid_array[i] = fork();
		
		if(pid_array[i] < 0)
		{
			perror("Fork: ");
			exit(EXIT_FAILURE);
		}

		if(pid_array[i] == 0)
		{
			// Wait for signal
			result = sigwait(&sigset, &sig);
			if(result == 0)
				printf("sigwait got signal: %d\n", sig);

			// Execute program
			prog = execvp(path, argList);
			
			if(prog == -1)
			{
				perror("Execvp: ");
			}

			exit(-1);
		}
		
			
		// Reset the argList
		for(int a = 0; a < k; a++)
		{
			argList[a] = NULL;
		}
		k = 0;
			
		i++;
			
		// Free larger tokens and reset variable
		free_command_line(&large_token_buffer);
		memset(&large_token_buffer, 0, 0);
	}

	sleep(5);
	// Send Sigsur1
	for(int a = 0; a < i; a++)
	{
		printf("Sending process %d SIGUSR1\n", pid_array[a]);
		kill(pid_array[a], SIGUSR1);
	}
	
	
	sleep(5);
	// Send Sigstop
	for(int a = 0; a < i; a++)
	{
		printf("Sending process %d SIGSTOP\n", pid_array[a]);
		kill(pid_array[a], SIGSTOP);
	}
	
	sleep(5);
	// Send Sigcont
	for(int a = 0; a < i; a++)
	{
		printf("Sending process %d SIGCONT\n", pid_array[a]);
		kill(pid_array[a], SIGCONT);
	}
	
	for(int a = 0; a < i; a++)
	{
		printf("Waiting on process %d\n", pid_array[a]);
		waitpid(pid_array[a], NULL, 0);
		printf("Process %d returned\n", pid_array[a]);
	}

	fclose(inputFile);
	free(line_buf);
	free(pid_array);
	
	exit(0);
}
