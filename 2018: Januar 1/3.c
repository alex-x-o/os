#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)

#define RD_END (0)
#define WR_END (1)
#define MAX_SIZE (256)

int main(int argc, char **argv)
{
	check(argc == 3, "argc");
	
	char *path = argv[1];
	char *arg = argv[2];
	
	int cld2par[2];
	check(pipe(cld2par) != -1, "pipe");
	
	pid_t childPid = fork();
	check(childPid != -1, "fork");
	
	if(childPid == 0)
	{
		close(cld2par[RD_END]);
		
		check(dup2(cld2par[WR_END], STDOUT_FILENO) != -1, "dup2");
		
		check(execlp("wc", "wc", path, arg, NULL) != -1, "execlp");
		
		close(cld2par[WR_END]);
		
		exit(EXIT_SUCCESS);
	}
	
	int status = 0;
	check(wait(&status) != -1, "wait");
	
	if(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))
	{
		close(cld2par[WR_END]);
		
		FILE *f = fdopen(cld2par[RD_END], "r");
		check(f != NULL, "fdopen");
		
		char *line = NULL;
		size_t lineLen = 0;
		
		while(getline(&line, &lineLen, f) != -1)
		{
			for(int i = 0; line[i] != '\0'; i++)
			{
				if(isspace(line[i]))
					break;
				printf("%c", line[i]);
			}
		}
		
		fclose(f);
		close(cld2par[RD_END]);
	}
	else
	{
		printf("Neuspeh");
	}
	
	return 0;
}
