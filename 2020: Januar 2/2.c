#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

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
	check(argc == 2, "argc");
	
	char *find = argv[1];
	int len = strlen(find);
	
	int pipeFd[2];
	check(pipe(pipeFd) != -1, "pipe");
	
	pid_t childPid = fork();
	check(childPid != -1, "fork");
	
	if(childPid > 0) // parent
	{
		close(pipeFd[WR_END]);
		
		FILE *f = fdopen(pipeFd[RD_END], "r");
		check(f != NULL, "fdopen");
		
		char *line = NULL;
		size_t lineLen = 0;
		char *dot;
		int count = 0;
		while(getline(&line, &lineLen, f) != -1)
		{
			dot = strrchr(line, '.');
			
			if(!strncmp(dot, find, len))
				count++;
		}
		printf("%d\n", count);
		
		close(pipeFd[RD_END]);
	}
	else
	{
		close(pipeFd[RD_END]);
		
		check(dup2(pipeFd[WR_END], STDOUT_FILENO) != -1, "dup2");
		
		check(execlp("find", "find", ".", "-type", "f", NULL) != -1, "execlp");
		
		close(pipeFd[WR_END]);
		
		exit(EXIT_SUCCESS);
	}
	int status = 0;
	check(wait(&status) != -1, "wait");
	
	if(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))
	{
		
	}
	else
	{
		exit(WEXITSTATUS(status));
	}
	
	return 0;
}
