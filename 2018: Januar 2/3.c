#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
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
	check(argc == 3, "argc");
	
	int pipeFd[2];
	check(pipe(pipeFd) != -1, "pipe");
	
	pid_t childPid = fork();
	check(childPid != -1, "fork");
	
	if(childPid == 0)
	{
		close(pipeFd[RD_END]);
		
		check(dup2(pipeFd[WR_END], STDOUT_FILENO) != -1, "dup2");
		
		check(execlp("tail", "tail", "-n", argv[2], argv[1], NULL) != -1, "execlp");
		
		close(pipeFd[WR_END]);
		
		exit(EXIT_SUCCESS);
	}
	int status = 0;
	check(wait(&status) != -1, "wait");
	
	if(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))
	{
		close(pipeFd[WR_END]);
	
		int readBytes = 0;
		char buffer[MAX_SIZE];
		
		while((readBytes = read(pipeFd[RD_END], buffer, MAX_SIZE)) > 0)
		{
			check(write(STDOUT_FILENO, buffer, readBytes) != -1, "write");
		}
		check(readBytes != -1, "read");
		
		close(pipeFd[RD_END]);
	}
	else
	{
		printf("Neuspeh");
	}
	
	return 0;
}
