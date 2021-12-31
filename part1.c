/*
 * part1.c
 *
 * Created on: Nov 3, 2021
 *     Author: Kelly Schombert
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
		printf("Error: %s does not exist. Must input name of currently existing file.\n", argv[2]);
		return 1;
	}

	char *line_buf = NULL;
	size_t len = 0;
	int i = 0;
	int k = 0;
	
	command_line large_token_buffer;
	
	char *token = NULL;
	char *path = NULL;
	char *argList[10];

	int numLines = 0;

	while(getline(&line_buf, &len, inputFile) != -1)
	{
		numLines++;
	}
	fclose(inputFile);

	inputFile = fopen(argv[1], "r");
	
	pid_t* pid_array;
	pid_array=(pid_t*)malloc(sizeof(pid_t) * numLines);
	
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
			if(execvp(path, argList) == -1)
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
