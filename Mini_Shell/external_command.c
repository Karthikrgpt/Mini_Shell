#include "minishell.h"

/*To Exicutes External commands*/
void execute_external_commands(char *input_string) {

    char buffer[MAX_COMMAND_LEN];
    strcpy(buffer, input_string); //cpying the command to do piped operation, if requisted
    
    char *argv[MAX_COMMAND] = {NULL};
    int i = 0, p_flag = 0;

    //geting the command using tocken
    char *token = strtok(input_string, " ");
    while (token) {
        
        argv[i++] = token;
        
        if (!strcmp(token, "|")) p_flag = 1; //checking for the piped commend
        if (!strcmp(token, ">")) p_flag = 1; //checking for the piped commend
        if (!strcmp(token, ">>")) p_flag = 1; //checking for the piped commend

        token = strtok(NULL, " "); //for next command
    }

    if (!p_flag) {

        execvp(argv[0], argv); //exicuting the command
    
        perror("execvp"); // Print error if execvp fails
        exit(EXIT_FAILURE);
    
    } else { // Handle piped commands
        
        FILE *fp;
        
        // Open a process using popen
        fp = popen(buffer, "r");
        if (fp == NULL) {
            perror("popen"); // Print error if popen fails
            exit(EXIT_FAILURE);
        }
        
        // Read and print the output of the command
        while (fgets(buffer, sizeof(buffer), fp)) printf("%s", buffer);

        
        pclose(fp); // Closing the process
        exit(EXIT_SUCCESS);
    }

}