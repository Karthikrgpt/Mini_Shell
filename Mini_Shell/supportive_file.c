#include "minishell.h"

static char string[BUFSIZ] = {'\0'}; //To store the command for the buffer application
int flag_prompt = 0; //to know the promt is default or user defined

/*to print the prompt*/
void print_prompt(void) {

	if (!flag_prompt)  //default prompt
		printf("\r"ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET":"ANSI_COLOR_BLUE"~%s"ANSI_COLOR_RESET"$ ",prompt,path + 12);
	else  //if user changed the prompt using PS1 command
		printf("\r%s", prompt);

    fflush(stdout); //clearing the output buffer

} 

/*To handel PS1 command*/
int handel_ps1_command(char *input_string) {

    if(strncmp("PS1=",input_string, 4) != 0) //Checking for PS1 command
        return FAILURE;

    if(!strchr(input_string, ' ')) { //if there is no space in the command

        strcpy(prompt,&input_string[4]); //copying the prompt
        flag_prompt = 1; //creating flag to print the new prompt 
        return SUCCESS;
    
    } else {
            
        int i = 0, j = 0;
        while (input_string[i++] != ' '); //traversing till space
        while (input_string[i]) input_string[j++] = input_string[i++]; //removing the first word
        input_string[j] = '\0'; //terminating with NULL
    }
}

/*To get command if multiple commands are requisted*/
char *get_command(char *input_string) {
    
	static char com[25] = {'\0'};
    static int index = 0;
	int j = 0;

	if ( input_string[j] != '\0') { //copying the entire string at first call
		strcpy(string, input_string);
		index = 0;
	}

	if (string[index] == '\0') { // checking for end of command
		index = 0;
		return NULL;
	}

	while (string[index] != '\0') {

		//checking for valid operators
		if (string[index] == '&' ) {
			if (string[index + 1] != '&') {
				printf("%c: command not found\n", string[index + 1]);
				return NULL;
			}
			break;
		}
	
		//copying the command and input string by the stored string
		com[j] = string[index];
		input_string[j] = string[index];
		j++; index++;
	
	}

	//terminating the both command and input string by the null charecter 
	input_string[j] = '\0';
	com[j] = '\0';

	//removing the charecter after the space in the command
	for (int i = 0; com[i]; i++)
		if (com[i] == ' ') {
			com[i] = '\0';
			break;
		}

	//skiping the charcters like &,| and space for next call
	while (string[index] == '&' || string[index] == '|' || string[index] == ' ' ) index++;

	//returning the command
	return com;
}

/*Extracting the external commands and storing it to an 2D character array */
void extract_external_commands(char **external_commands) {

    char ch;
    char buffer[25] = {'\0'};
    int i = 0 , j = 0;

    int fd = open("external.txt",O_RDONLY); //Opening the file
    if(fd == -1) {
        printf("open : Failed");
        exit(1);
    }
    
    while( read( fd, &ch, 1 ) > 0) { //Reding the character by character
        
        if(ch != '\n') { //copy till it read new line
        
            buffer[i++] = ch;
        
        } else { //if new line occers, store it to the array
            
            external_commands[j] = calloc( strlen(buffer)+1, sizeof(char) ); //memmory allocation
            strcpy(external_commands[j++],buffer); //copy the contents
            external_commands[j] = '\0'; //terminating with NULL

            memset(buffer,'\0',25); //clear the buffer
            i = 0; //update the index
        }
    }

}

/*To insert at first node for blocked commands*/
int insert_first(stoped_command **head, char *blocked_command, int sig_num_count) {
	
	//creating the new node
    stoped_command *new = (stoped_command *) malloc (sizeof(stoped_command));
    if (new == NULL) EXIT_FAILURE;

	strcpy(new->command, blocked_command); //copy the command
	strcpy(new->status, "Stopped"); //update the status as Stoped
	new->count = sig_num_count; //store the count
    new->link = NULL; 
    
	printf("\r^Z\n[%d]+  %-10s%20s\n", sig_num_count, new->status, blocked_command); //print the stored command
	memset(string,'\0',25); //clear the memory

    if ( *head == NULL ) { //if list is empty
        *head = new; 
        return EXIT_SUCCESS;
    }

    new->link = *head; //else store it as first node
    *head = new;

    return EXIT_SUCCESS;
}

/*to count how many times that substring present in the string*/
/*used for fg and bg process, to know how many times it requested*/
int count_Substring_Occurrences(char *str, char *substr) {

    int count = 0;
    int substr_len = strlen(substr); //geting the length
    const char *temp = str;
    
    // Loop through the string to find occurrences
    while ((temp = strstr(temp, substr)) != NULL) {
        count++;
        temp += substr_len; // Move past the found substring
    }
    
    return count; //returning the count
}

/*to get the requested command in fg and bg process*/
/*if "fg fg" is requestd then count will be 2 and it will retuen the 2 node data*/
stoped_command *get_blocked_command(stoped_command *head, int position) {

    stoped_command *temp = head;
    int count = 1;

    while (temp != NULL) { //traversing the end

        if (count == position) return temp; //find the correct position
        
		temp = temp->link; //move to next node
        count++; //updating the count
    }

    return NULL; //returning the null if no data found
}
