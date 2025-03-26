#include "minishell.h"

/*To handel echo command*/
void echo(char *input_string, int status) {
    
    if(!strncmp("echo $?",input_string,7)) printf("%d\n",status); //exit status of last child process

    else if(!strncmp("echo $$",input_string,7)) printf("%d\n",getpid()); //PID of the minishell project

    else if(!strncmp("echo $SHELL",input_string,11)) printf("%s\n",getenv("SHELL")); //echo $SHELL

    else if(!strncmp("echo",input_string,4)) printf("%s\n",input_string + 5); //if user enter any other commands

}

/*To exexute the inpternal commands*/
void execute_internal_commands(char *input_string) {

    if(!strncmp("exit",input_string,4)) exit(EXIT_SUCCESS); //exit-->terminate the program

    else if(!strncmp("pwd",input_string,3)) { //for pwd
        getcwd(path, sizeof(path));
        printf("%s\n",path);
    }

    else if(!strncmp("cd",input_string,2)) { // for cd

        int i,count = 0;
        for(i = 2 ; input_string[i] == ' ' ; i++) count++; //jumping to the directory path
    
        chdir(&input_string[ 2 + count ]); //changing the directory

        getcwd(path, sizeof(path)); // Get the current directory
    }
}