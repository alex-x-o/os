#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

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
	check(argc == 2, "argc");
	
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
		int k, i = 0;
		char *space;
		char *res = malloc(6 * sizeof(char));
		check(res != NULL, "malloc");
		
		while(getline(&line, &lineLen, f) != -1)
		{
			if(i == 0)
			{
				i++;
			}
			else
			{
				int len = strlen(line);
				space = strrchr(line, ' ');
				int lenSpace = strlen(space + 1);
				check(space != NULL, "strrchr");
				k = 6;
				
				for(int j = 0; j < 6; j++)
				{
					res[j] = line[len - lenSpace - k];
					k--;
				}
					
				printf("%s\n", res);
			}
		}
		
		close(pipeFd[RD_END]);
	}
	else
	{
		printf("neuspeh");
	}
	
	return 0;
}
