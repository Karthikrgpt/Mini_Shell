#include "minishell.h"

int main() {

	//clearing the system out
	system("clear");

	//Naming the minishell
	strcpy(prompt, "minishell");

	//Geting the path for the prompt
	strcpy(path,getenv("PWD")); 

	//extracting the external commands stored in the external.txt file
	extract_external_commands(external_command);

	//scaning the commands from the user
	scan_input();

	exit(EXIT_SUCCESS);
}
