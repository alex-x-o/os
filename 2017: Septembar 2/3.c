#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define RD_END (0)
#define WR_END (1)
#define MAX_SIZE (256)

int main(int argc, char **argv)
{
	check(argc == 1, "./3");
	
	int a, b;
	char c;
	while(scanf("%d %d %c", &a, &b, &c) != EOF)
	{
		int cld2par[2];
		check(pipe(cld2par) != -1, "pipe");
		
		int childPid = fork();
		check(childPid != -1, "fork");
		
		if(childPid > 0) //parent
		{
			close(cld2par[WR_END]);
			
			FILE *f = fdopen(cld2par[RD_END], "r");
			check(f != NULL, "fdopen");
			
			char *line = NULL;
			size_t lineLen = 0;
			
			while(getline(&line, &lineLen, f) != -1)
			{
				line[lineLen] = '\0';
				printf("%s", line);
			}
			
			//close(cld2par[RD_END]);
		}
		else //child
		{
			close(cld2par[RD_END]);
			
			check(dup2(cld2par[WR_END], STDOUT_FILENO) != -1, "dup2");
			
			char x[MAX_SIZE];
			char y[MAX_SIZE];
			char op[MAX_SIZE];
			
			sprintf(x, "%d", a);
			sprintf(y, "%d", b);
			sprintf(op, "%c", c);
						
			check(execlp("expr", "expr", x, op, y, NULL) != -1, "execlp");
			
			//close(cld2par[WR_END]);
			
// 			exit(EXIT_SUCCESS);
			wait(0);
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
	}
	
	return 0;
}
