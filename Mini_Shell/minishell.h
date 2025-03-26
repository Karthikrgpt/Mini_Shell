#ifndef MINISHELL_H
#define MINISHELL_H

/*Header declaretion*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h> //for __fpurge(stdin) to clear the input buffer
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>

/*Macro declaretion*/
#define MAX_COMMAND		50
#define MAX_COMMAND_LEN		100

#define FAILURE		-1
#define SUCCESS		0
#define BUILTIN		1 //to return the built in command
#define EXTERNAL	2 //to return the external in command
#define NO_COMMAND  3 //to return the invalid command

#define ANSI_COLOR_GREEN   "\033[1;32m"
#define ANSI_COLOR_BLUE    "\033[1;34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*List of builtin commands*/
static char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
	            "set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
				"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};

/*Structure for the blocked commands*/
typedef struct s_data {
	char command[MAX_COMMAND]; //to store the command
	char status[10]; //to store the status of command i,e. stoped or completed
	int count; //to store the node/command number
	struct s_data *link; //to store the address of next node
} stoped_command;

/*Global declaretion*/
char *external_command[200]; //to stroe the external commands
char path[MAX_COMMAND]; //to store the path of the current working directory
char prompt[MAX_COMMAND]; //to store the prompt
pid_t pid; //to store the pid of the process 

/*to scan the input command from the user*/
void scan_input(void);

/*To get command if multiple commands are requisted*/
char *get_command(char *input_string);

/*To check the type of command requested like bulitin or external*/
int check_command_type(char *command);

/*To handle the echo commands*/
void echo(char *input_string, int status);

/*To handel PS1 command*/
int handel_ps1_command(char *input_string);

/*To handle the foreground and background process*/
int fg_bg_process(char *input_string, stoped_command **head);

/*To execute the internal commands*/
void execute_internal_commands(char *input_string);

/*To execute the external commands*/
void execute_external_commands(char *input_string);

/*Extracting the external commands and storing it to an 2D character array */
void extract_external_commands(char **external_commands);

/*To handle the signal for ctrl + c and ctrl + z command*/
void signal_handler(int sig_num);

/*to print the prompt*/
void print_prompt(void);

/*To insert at first node for blocked commands*/
int insert_first(stoped_command **head, char *blocked_command, int sig_num_count);

/*to count how many times that substring present in the string*/
/*used for fg and bg process, to know how many times it requested*/
int count_Substring_Occurrences(char *str, char *substr);

/*to get the requested command in fg and bg process*/
/*if "fg fg" is requestd then count will be 2 and it will retuen the 2 node data*/
stoped_command *get_blocked_command(stoped_command *head, int position);

#endif