#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string.h>
#include <ctype.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define RD_END (0)
#define WR_END (1)

int main(int argc, char **argv)
{
	check(argc == 2, "Usage: ./2 pathToFile");
	
	int cld2par[2];
	check(pipe(cld2par) != -1, "pipe");
	
	pid_t childPid = fork();
	check(childPid != -1, "fork");
	
	if(childPid > 0) // parent
	{
		close(cld2par[WR_END]);
		
		FILE *f = fdopen(cld2par[RD_END], "r");
		check(f != NULL, "fdopen");
		
		char *line = NULL;
		size_t lineLen = 0;
		
		while(getline(&line, &lineLen, f) != -1)
		{
			char *sub;
			char *needle = "Size: ";
			sub = strstr(line, needle);
			if(sub != NULL)
			{
				size_t size = strlen(needle);
				sub += size;
				while(!isspace((unsigned char)*sub))
				{
					printf("%c", *sub);
					sub++;
				}
			}
		}
		
		close(cld2par[RD_END]);
	}
	else //child
	{
		close(cld2par[RD_END]);
		
		check(dup2(cld2par[WR_END], STDOUT_FILENO) != -1, "dup2");
		
		check(execlp("stat", "stat", argv[1], NULL) != -1, "execlp");
		
		close(cld2par[WR_END]);
		
		exit(EXIT_SUCCESS);
	}
	
	int status = 0;
	check(wait(&status) != -1, "wait");
	
	if(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))
	{
		
	}
	else
	{
		printf("Neuspeh");
	}
	
	
	return 0;
}
