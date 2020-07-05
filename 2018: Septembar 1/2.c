#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define check(expr, msg) \
	do{ \
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
	check(argc > 1, "argc");
	
	int pipeFd[2];
	check(pipe(pipeFd) != -1, "pipe");
	
	pid_t childPid = fork();
	check(childPid != -1, "fork");
	
	if(childPid == 0)
	{
		close(pipeFd[RD_END]);
		
		check(dup2(pipeFd[WR_END], STDOUT_FILENO) != -1, "dup2");
		
		int n = argc;
		char **args = malloc(n * sizeof(char*));
		check(args != NULL, "malloc");
		int i;
		for(i = 1; i < n; i++)
		{
			args[i-1] = malloc(strlen(argv[i] + 1));
			check(args[i-1] != NULL, "malloc");

			strcpy(args[i-1], argv[i]);
		}
		args[i-1] = NULL;
		
		check(execvp(argv[1], args) != -1, "execvp");
		
		close(pipeFd[WR_END]);
		
		exit(EXIT_SUCCESS);
	}
	
	int status = 0;
	check(wait(&status) != -1, "wait");
	
	if(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))
	{
		close(pipeFd[WR_END]);
		
		char *line = NULL;
		size_t lineLen = 0;
		FILE *f = fdopen(pipeFd[RD_END], "r");
		check(f != NULL, "fdopen");
		int num = 0;
		
		while(getline(&line, &lineLen, f) != -1)
		{
			num++;
		}
		printf("%d\n", num);
		
		close(pipeFd[RD_END]);
	}
	else
	{
		printf("Neuspeh");
	}
	
	return 0;
}
