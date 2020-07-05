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
	
int main(int argc, char** argv)
{
	check(argc == 1, "argc");
	
	int pipeFd[2];
	check(pipe(pipeFd) != -1, "pipe");
	
	pid_t childPid = fork();
	check(childPid != -1, "fork");
	
	if(childPid == 0) //child
	{
		close(pipeFd[RD_END]);
				
		check(dup2(pipeFd[WR_END], STDOUT_FILENO) != -1, "dup2");
		
		check(execlp("ls", "ls", "-l", NULL) != -1, "execlp");
		
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
		
		while(getline(&line, &lineLen, f) != -1)
		{
			if(line[7] == 'r' && line[8] == 'w' && line[9] == 'x')
			{
				char *space = strrchr(line, ' ');
				printf("%s", space + 1);
			}
		}
		
		free(line);
		fclose(f);
		close(pipeFd[RD_END]);
	}
	else
	{
		printf("neuspeh");
		exit(WEXITSTATUS(status));
	}
	
	return 0;
}
