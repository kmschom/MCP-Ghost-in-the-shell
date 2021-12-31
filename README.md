# MCP-Ghost-in-the-shell
CIS 415 Project 2: a Master Control Program a.k.a Ghost-in-the-shell

MCP Ghost in the Shell launches a pool of sub-processes that will execute commands given in an input file. 
MCP reads list of commands (with arguments) to run from a file, runs the process that executes command, then schedules the processes to run concurrently in a time-sliced manner. 
MCP monitors the processes, keeping track of how the processes are using system resources. 

Project Details in Project-2-Description.docx.pdf

The creator of each file is noted at the top of the file along with it's date of creation.

part1.c: c-file containing MCP v1.0.

part2.c: c-file containing MCP v2.0. 

part3.c: c-file containing MCP v3.0.

part4.c: c-file containing MCP v4.0. 

string_parser.h: A header file that contains user-defined command_line struct and functions headers related to them.

string_parser.c: This file contains all the functions for the string_parser header file.

cpubound.c: example process for the MCP to call

iobound.c: example process for the MCP to call

tar.txt: command line to tar files for submission
