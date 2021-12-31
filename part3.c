/*
 * part3.c
 *
 * Created on: Nov 9, 2021
 *     Author: Kelly Schombert
 *
 * Copied from part2 on Nov. 9
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

void on_sigalarm(int sig)
{
	printf("\nALARM signal received: %d\n\n", sig);
	raise(SIGUSR1);

}

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
	sigset_t chsigset;
	sigemptyset(&chsigset);
	sigaddset(&chsigset, SIGCONT);
	sigprocmask(SIG_BLOCK, &chsigset, NULL);

	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	signal(SIGALRM, &on_sigalarm);
	
	int sig;
	int msig;
	int result;
	int mresult;
	
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
			result = sigwait(&chsigset, &sig);
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

	int status = 1;
	int start = 0;
	int fin = 0;
	int curr = 0;
	int flag = 0;
	pid_t* fin_array;
	fin_array=(pid_t*)malloc(sizeof(pid_t) * numLines);

	alarm(1);
	while(1)
	{
		mresult = sigwait(&sigset, &msig);
		if(mresult == 0)
		{

			printf("Waiting on process %d\n", pid_array[curr]);
			waitpid(pid_array[curr], &status, WNOHANG);
			// Trying to check if child processes terminated
			if(WIFEXITED(status))
			{
				// Try to check if child process terminated naturally or was stopped by a signal	
				if(!WIFSIGNALED(status))
				{
					printf("Process %d finished\n", pid_array[curr]);
					fin_array[curr] = pid_array[curr];
					pid_array[curr] = NULL;
				
					for(int a = curr; a < numLines; a++)
					{
						if(pid_array[a] != NULL)
						{
							curr = a;
							printf("Starting next process: Process %d\n", pid_array[curr]);
							kill(pid_array[curr], SIGCONT);
							alarm(1);
							flag = 1;
							break;
						}
					}
					if(flag == 0)
					{
						for(int a = 0; a < numLines; a++)
						{
							if(pid_array[a] != NULL)
							{
								curr = a;
								printf("Starting next process: Process %d\n", pid_array[curr]);
								kill(pid_array[curr], SIGCONT);
								alarm(1);
								flag = 1;
								break;
							}
						}
					}
				
					if(flag == 0)
					{
						printf("All processes finished\n");
						fclose(inputFile);
						free(line_buf);
						free(pid_array);
						free(fin_array);
						break;
						//exit(0);
					}
					flag = 0;
				}else
				{
					printf("Stopping process %d\n", pid_array[curr]);
					kill(pid_array[curr], SIGSTOP);
					for(int a = curr; a < numLines; a++)
					{
						if(pid_array[a] != NULL)
						{
							curr = a;
							printf("Starting next process: Process %d\n", pid_array[curr]);
							kill(pid_array[curr], SIGCONT);
							alarm(1);
							flag = 1;
							break;
						}
					}
					if(flag == 0)
					{
						for(int a = 0; a < numLines; a++)
						{
							if(pid_array[a] != NULL)
							{
								curr = a;
								printf("Starting next process: Process %d\n", pid_array[curr]);
								kill(pid_array[curr], SIGCONT);
								alarm(1);
								flag = 1;
								break;
							}
						}
					}
					if(flag == 0)
					{
						printf("All process finished\n");
						fclose(inputFile);
						free(line_buf);
						free(pid_array);
						free(fin_array);
						break;
						//exit(0);
					}
					flag = 0;
				}
			}else
			{	
				// If waitpid() didn't recieved any update, then 1st process hasn't been started
				if(start == 0)
				{
					kill(pid_array[0], SIGCONT);
					alarm(1);
					start = 1;
				}else
				{
					fprintf(stderr, "Error: Process didn't go\n");
					fclose(inputFile);
					free(line_buf);
					free(pid_array);
					free(fin_array);
					exit(-1);
				}
			}
		}else
		{
			fprintf(stderr, "Error: MCP did not receive signal\n");
			fclose(inputFile);
			free(line_buf);
			free(pid_array);
			free(fin_array);
			exit(-1);
		}
	}

	// Having program wait for processes as it won't finish correctly

	for(int a = 0; a < i; a++)
	{
		//printf("Waiting on process %d\n", pid_array[a]);
		waitpid(pid_array[a], NULL, 0);
		//printf("Process %d returned\n", pid_array[a]);
	}

	exit(0);
}
