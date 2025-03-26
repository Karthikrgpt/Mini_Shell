#include "minishell.h"

extern int sig_num_count;
stoped_command *command_s; // To store the command requested (UDT)

int fg_process(); // To execute fg process
void bg_process(); // To execute bg process
void print_reverse(stoped_command *head); // To execute jobs command
void delete_node(stoped_command **head, int position); //to delete the commands
int get_node_number(stoped_command *head);     //finding the number of nodes

/* Handle foreground and background processes */
int fg_bg_process(char *input_string, stoped_command **head) {
    
    if (strstr(input_string, "fg") == NULL && 
        strstr(input_string, "bg") == NULL && 
        strstr(input_string, "jobs") == NULL)
        return FAILURE;

    //checking if three are commands are not
    if (*head == NULL && strstr(input_string, "jobs") != NULL) return SUCCESS;
    
    if (!strcmp(input_string, "jobs")) { // Print stored blocked processes
        print_reverse(*head);
        return SUCCESS;
    }

    /*checking which command is requested for*/
    int count_fg = count_Substring_Occurrences(input_string, "fg");
    int count_bg = count_Substring_Occurrences(input_string, "bg");

    /*finding the number of nodes*/
    int node_count = get_node_number(*head);

    if (count_fg <= node_count && !count_bg) { // Foreground requested
    
        command_s = get_blocked_command(*head, count_fg);
        if (fg_process() == SUCCESS) delete_node(head, count_fg);
        else delete_node(head, 1);
        return SUCCESS;
    
    } else if (!count_fg && count_bg <= node_count) { // Background requested
    
        command_s = get_blocked_command(*head, count_bg);
        bg_process();
        return SUCCESS;
    
    }
    
    printf("bash: %s: current: no such job\n", input_string);
    return SUCCESS; // Continue processing other commands
}

/* Signal handler for Ctrl + C and Ctrl + Z */
void signal_handler_fg_bg(int sig_num) {
    
    if (sig_num == SIGINT)  printf("\n"); // New line for Ctrl + C
    else if (sig_num == SIGTSTP)  printf("\33[2K"); // Clear current line in stdout
    
    print_prompt();
}

/* Foreground process execution */
int fg_process() {
    
    printf("%s\n", command_s->command);

    pid_t pid1 = fork();
    if (pid1 == 0) { // Child process
    
        signal(SIGINT, SIG_DFL);  // Reset signals to default
        signal(SIGTSTP, SIG_DFL);
        
        char *args[] = { "/bin/sh", "-c", command_s->command, NULL }; // Execute command
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } 

    // Parent process (Wait for child to finish)
    int status;
    waitpid(pid1, &status, WUNTRACED);

    if (WIFEXITED(status) || WIFSIGNALED(status)) {
        strcpy(command_s->status, "Done");
    } else if (WIFSTOPPED(status)) {
        strcpy(command_s->status, "Stopped");
        return FAILURE;
    }

    return SUCCESS;
}

/* Background process execution */
void bg_process() {
    
    printf("[%d]  %s\n", command_s->count, command_s->command);

    pid_t pid1 = fork();
    if (pid1 == 0) { // Child process
        setpgid(0, 0);  // Create new process group (Prevents parent signals)
        
        signal(SIGINT, signal_handler_fg_bg);   //selected as user defined
        signal(SIGTSTP, signal_handler_fg_bg);  

        char *args[] = { "/bin/sh", "-c", command_s->command, NULL }; // Execute command
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } 

    // Parent process should NOT wait, just update the status
    waitpid(pid, NULL, WNOHANG); //helps to clear the resorces
    strcpy(command_s->status, "Done"); //update the status
}

/* Print the blocked processes (jobs command) */
void print_reverse(stoped_command *head) {

    if (head == NULL) return;
    
    print_reverse(head->link); // Recursively print in reverse order
    printf("[%d]   %-10s%20s\n", head->count, head->status, head->command);

}

// Function to delete a node at a given position
void delete_node(stoped_command **head, int position) {

    if (*head == NULL) {
        printf("(NULL)\n");
        return;
    }

    stoped_command *temp = *head;
    sig_num_count--;

    // If head needs to be removed
    if (position == 1) {
        *head = temp->link;
        free(temp);
        return;
    }

    // Find previous node of the node to be deleted
    stoped_command *prev = NULL;
    for (int i = 1; temp != NULL && i < position; i++) {
        prev = temp;
        temp = temp->link;
    }

    // If position is greater than number of nodes
    if (temp == NULL) return;

    // Unlink the node from linked list
    prev->link = temp->link;
    free(temp);
}

/*finding the number of nodes*/
int get_node_number(stoped_command *head) {
    
    int node_count = 0;
    stoped_command *temp= head;

    while (temp) {
        node_count++;
        temp = temp->link;
    }

    return node_count;
}