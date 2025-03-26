#include "minishell.h"

int sig_num_count = 0; //for command count
char input_string[BUFSIZ]; //to store the input entered by the user
stoped_command *command_head = NULL; //to store the blocked commands using ctrl + z

/*To scan the input and procces it later*/
void scan_input() {

    char *command = "clear"; //storing the initial command as clear
    int status;

    signal(SIGINT, signal_handler); //to handel crlt + c and directing signal to user defined signal_handler
    signal(SIGTSTP, signal_handler); //to handel crlt + z and directing signal to user defined signal_handler

	while(1) {

        memset(input_string,'\0',25); //clear the memory
        
        print_prompt(); //printing the prompt
 
		fflush(stdout); //clear the output buffer
        scanf("%[^\n]s",input_string);
        __fpurge(stdin); //clear the input buffer

        //to handel PS1 command
        if (handel_ps1_command(input_string) == SUCCESS) continue;

        //for foreground and background process manegement
        if (fg_bg_process(input_string, &command_head) == SUCCESS) continue;

        command = get_command(input_string); //getting commands using the strtok

        while ( command != NULL) {
            
            /*Checking the type of command like built in or external command or none*/
            switch (check_command_type(command)) {

                /*For the External Command*/
                case EXTERNAL:                
                    pid = fork();
                    if(pid > 0 ) waitpid(pid, &status, WUNTRACED); //parent process waiting till cheld completes 
                    else {
                        signal(SIGINT, SIG_DFL); //to handel crlt + c and directing signal to user defined default handler
                        signal(SIGTSTP, SIG_DFL); //to handel crlt + z and directing signal to user defined default handler
                        execute_external_commands(input_string); //firts child will complete
                    }
                    break;
                
                /*For the Built-In Command*/
                case BUILTIN:
                    echo(input_string,status); //Implement call the family of echo functions
                    execute_internal_commands(input_string); //Implement internal commands
                    break;

                /*For In-vaild Case*/
                default: printf("%s: command not found\n", command);
            }

            memset(input_string, '\0', BUFSIZ); //clearing the input string
            command = get_command(input_string); //geting the next command, If there and Passing NULL to get next
        }
    }
}

/*Handling the signal for ctrl + c and ctrl + z*/
void signal_handler(int sig_num) {

	if (sig_num == SIGINT) printf("\n"); //new line for only crlt + c
    else if (sig_num == SIGTSTP) { //clear current line for only crlt + z
        
        printf("\33[2K"); //to clear the current working line in stdout

        /*if user try to block the child process, then it will store the command for future use*/
        if (input_string[0] != '\0') insert_first(&command_head, input_string, ++sig_num_count);
    }

    print_prompt(); //to print the prompt
}

/*To check the requested type of command i.e, builtin or external*/
int check_command_type(char *command) {

    //checking for Bultin commands
    for(int i = 0 ; builtins[i] != NULL ; i++) if(!strcmp(command,builtins[i])) return BUILTIN;
    
    //checking for empty commands
    if(strcmp(command,"\0") == 0) return NO_COMMAND;       
    
    //checking for External commands
    for(int i = 0 ; external_command[i] != NULL ; i++) if(!strcmp(command,external_command[i])) return EXTERNAL;

}