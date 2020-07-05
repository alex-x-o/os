#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define RD_END (0)
#define WR_END (1)
#define MAX_SIZE (4094)
#define MAX_KEKE (128)
	
char *maxCommand;
char *maxArgument;
int maxBytes = 0;

int main(int argc, char **argv)
{
	check(argc == 2, "argc");
	
	FILE *f = fopen(argv[1], "r");
	check(f != NULL, "fopen");
	
	char *line = NULL;
	size_t lineLen = 0;
	
	int oldFd = dup(STDOUT_FILENO);
	
	while(getline(&line, &lineLen, f) != -1)
	{
		int cld2par[2];
		check(pipe(cld2par) != -1, "pipe");
	
		//printf("%s\n", line);
		char *command = malloc(strlen(line));
		check(command != NULL, "malloc");
		char *argument = malloc(strlen(line));
		check(argument != NULL, "malloc");
		
		int i;
		for(i = 0; line[i] != '\0'; i++)
		{
			if(isspace(line[i]))
				break;
			command[i] = line[i];
		}
		command[i] = '\0';
		
		char *space = strchr(line, ' ');
		space = space + 1;
			
		for(i = 0; space[i] != '\0'; i++)
		{
			if(isspace(space[i]))
				break;
			argument[i] = space[i];
		}
		argument[i] = '\0';
		
		pid_t childPid = fork();
		check(childPid != -1, "fork");
	
		if(childPid == 0) //child
		{
			close(cld2par[RD_END]);
			
			check(dup2(cld2par[WR_END], STDOUT_FILENO) != -1, "dup2");
			
			check(execlp(command, command, argument, NULL) != -1, "execlp");
			
			check(dup2(oldFd, STDOUT_FILENO) != -1, "dup2");
			
			close(cld2par[WR_END]);
			
			exit(EXIT_SUCCESS);
		}
		close(cld2par[WR_END]);
		
		int status = 0;
		check(wait(&status) != -1, "wait");
		
		if(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))
		{
			char buffer[MAX_SIZE];
			int readBytes = 0;
						
			while((readBytes = read(cld2par[RD_END], buffer, MAX_SIZE)) > 0)
			{
				if(readBytes > maxBytes)
				{
					maxBytes = readBytes;
					maxCommand = command;
					maxArgument = argument;
				}
				//check(write(fd, buffer, readBytes) != -1, "write");
			}
			check(readBytes != -1, "read");
		}
		else
		{
			
		}
		close(cld2par[RD_END]);
	}
	
	
	printf("%s %s\n", maxCommand, maxArgument);
	
	fclose(f);
	return 0;
}
