//My Shell - Dimitrios-Marios Exarchou 8805 .

//Libraries. 
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/wait.h>
#include  <signal.h>
#include  <fcntl.h>

//Constants.
#define MAXCHARS 512
#define MAXARGS 256 // MAXCHARS/2 ,because of the spaces
#define BUFSIZE 1024

//Functions. 
void init_shell();

int shell_function(int argc, char **argv);
void print_prompt();
void take_input(char **string);
void parse_input(char *string, char ***tokens, int *args);
void execute_input(char **tokens, int args);
int execBasic(char **tokens, int args); //return 1 if child runs properly
void execSemicolon(char **tokens ,int args,int delimiter);
void execAmpersand(char **tokens ,int args,int delimiter);

void quit_shell();



//Main Programm.
int main(int argc, char **argv)
{
	char *inputString;
	char **inputTokens;
	int flag, arguments;

	init_shell();
	flag = shell_function(argc, argv);

	while(1)
	{
		if (!flag) //Interactive function - Avoid printing prompt in batch function.
		{
			print_prompt();
		}

		take_input(&inputString);

		parse_input(inputString, &inputTokens, &arguments);

		execute_input(inputTokens, arguments);
		

		free(inputString);
		free(inputTokens);
	}

	return 0;
}



//Function to greet user.
void init_shell()
{
	printf("\n\n### Welcome to my shell! Use at your own risk! ###\n\n");
}



//Function to decide input.
int shell_function(int argc, char **argv)
{
	if (argc == 2) //file + NULL
	{
		FILE *fd;
		fd = fopen(argv[1], "r");
		if (fd == NULL)
		{
			printf("Error: Could not find batchfile.\n");
			exit(0);
		}

		dup2(fileno(fd),0); // redirecting stdin(0) to fd file

		return 1;
	}
	else if(argc > 2)
	{
		printf("Error: Too many arguments.\n");
		exit(0);
	}

	return 0;
}



//Function to print prompt.
void print_prompt()
{
	printf("exarchou_8805> ");
}



//Function to take input.
void take_input(char **string)
{
	*string = (char *) malloc(BUFSIZE * sizeof(char));
	if (!(*string))
	{
		printf("Error 1: malloc() failed.\n");
		exit(0);
	}

	if (fgets(*string, BUFSIZE, stdin) != NULL)
	{
		if (strlen(*string) > MAXCHARS +1) //MAXCHARS + NULL
		{
			printf("Error: Too many characters.\n");
			**string = '\0';
		}
	}
	else
	{
		printf("Error: fgets() failed.\n");
		exit(0);
	}

}



//Function to parse input.
void parse_input(char *string, char ***tokens, int *args)
{
	*tokens = (char **) malloc((MAXARGS + 1) * sizeof(char *));
	if (!(tokens))
	{
		printf("Error 2: malloc() failed.\n");
		exit(0);
	}

	int index = 0;

	(*tokens)[index] = strtok(string, " \n\t\a\r");

	while ( (*tokens)[index] != NULL )
	{
		index++;
		(*tokens)[index] = strtok(NULL, " \n\t\a\r");
	}

	(*tokens)[index] = NULL;
	*args = index; 
}



//Function to execute input.
void execute_input(char **tokens, int args)
{

	if (tokens[0] == NULL) //Empty line.
	{
		return ;
	}

	if (strcmp(tokens[0], "quit") == 0) //Exit Shell.
	{
		quit_shell();
	}
	

	int flag = 0;
	int delimiter;

	//Search for ; and && delimiters.
	for (int i=0; i<args; i++)
	{
		if (strcmp(tokens[i], ";") == 0)
		{
			if (!i) //delimiter in first postion.
			{
				printf("Syntax Error.\n");
				return;
			}

			flag = 1;
			delimiter = i;
			break;
		}

		if (strcmp(tokens[i], "&&") == 0)
		{
			if (!i) //delimiter in first postion.
			{
				printf("Syntax Error.\n");
				return;
			}

			flag = 2;
			delimiter = i;
			break;
		}
	}

	//Decide type of execution.
	if (flag == 0)
	{
		execBasic(tokens, args);
	}
	else if (flag == 1)
	{
		execSemicolon(tokens , args, delimiter);
	}
	else
	{
		execAmpersand(tokens , args, delimiter);
	}

}



//Function to execute simple command without any delimiter.
int execBasic(char **tokens, int args)
{

	int status;
	pid_t pid = fork(); //Generating a child.

	if (pid == 0) //Child process.
	{
		//Search for redirection delimiters.
		for(int i=0; i<args; i++)
		{
			if (strcmp(tokens[i], "<") == 0)
			{
				char *file = tokens[i+1];
				if (file == NULL)
				{
					printf("Error: Cannot find file to read.\n");
					exit(100);
				}

				tokens[args-2] = NULL; //Skiping strings < and NULL.

				FILE *fd;
				fd = fopen(file, "r");

				if (fd == NULL)
				{
					printf("Error: Cannot open file to read.\n");
					exit(100);
				}

				dup2(fileno(fd), 0); //Redirecting input.
				break;
			}

			if (strcmp(tokens[i], ">") == 0)
			{
				char *file = tokens[i+1];
				if (file == NULL)
				{
					printf("Error: Cannot find file to write.\n");
					exit(100);
				}

				tokens[args-2] = NULL; //Skiping strings > and NULL.

				FILE *fd;
				fd = fopen(file, "w");

				if (fd == NULL)
				{
					printf("Error: Cannot open file to write.\n");
					exit(100);
				}

				dup2(fileno(fd), 1); //Redirecting output.
				break;
			}

		}

		if ( execvp(tokens[0], tokens) < 0)
		{
			printf("Error: execvp() failed.\n");
			exit(100);
		}

	}
	else if (pid > 0) //Mother process.
	{
		wait(&status);
  		if (WIFEXITED(status) && (WEXITSTATUS(status) != 100))
  		{
  			return 1;
  		}
  		return 0;
  	}		
	else
	{
		printf("Error: fork() failed.\n");
		exit(0);
	}
	
}



//Function to handle Semicolon.
void execSemicolon(char **tokens ,int args,int delimiter)
{
	tokens[delimiter] = NULL;

	execBasic(&tokens[0], delimiter);

	execute_input(&tokens[delimiter +1], args-delimiter-1); //1 is beacuse of the delimiter character.
}



//Function to handle Ampersand.
void execAmpersand(char **tokens ,int args,int delimiter)
{
	tokens[delimiter] = NULL;

	if (execBasic(&tokens[0], delimiter) == 1)
	{
		execute_input(&tokens[delimiter +1], args-delimiter-1);
	}
}



//Function to handle quit.
void quit_shell()
{
	printf("Goodbye!\n");
	exit(0);
}


