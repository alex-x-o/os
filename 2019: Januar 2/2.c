#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

#define check_error(expr, msg) \
	do { \
		if(!(expr)){ \
			fprintf(stderr, "%s\n", msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define READ_END (0)
#define WRITE_END (1)

int main(int argc, char **argv)
{
	check_error(argc >= 3, "wrong num of arg");
	
	for(int i = 1; i < argc; i++)
	{
		int cld2par[2];
		check_error(pipe(cld2par) != -1, "pipe");
		
		pid_t childPid = fork();
		check_error(childPid != -1, "fork");
		
		if(childPid == 0) // child
		{
			close(cld2par[READ_END]);
			
			check_error(dup2(cld2par[WRITE_END], STDOUT_FILENO) != -1, "dup2");
			
			struct stat fInfo;
			if(stat(argv[i], &fInfo) == -1)
				exit(EXIT_FAILURE);
			
			check_error(execlp("du", "du", "-sch", argv[i], NULL) != -1, "execlp");
			
			close(cld2par[WRITE_END]);
			
			exit(EXIT_SUCCESS);
		}
		
		int status = 0;
		check_error(wait(&status) != -1, "wait");
			
		if(WIFEXITED(status) && (WEXITSTATUS(status) == EXIT_SUCCESS))
		{
			close(cld2par[WRITE_END]);
			
			FILE *f = fdopen(cld2par[READ_END], "r");
			check_error(f != NULL, "fdopen");
			
			char *line = NULL;
			size_t lineLen = 0;
			int lineNum = 1;
			
			while(getline(&line, &lineLen, f) != -1)
			{
				if(lineNum != 1)
					break;
				
				while(!isspace((unsigned char)*line))
				{
					printf("%c", *line);
					line++;
					lineNum++;
				}
				
				if(i != argc - 1)
					printf(" ");
			}
			
			close(cld2par[READ_END]);
		}
		else
		{
			printf("neuspeh");
			if(i != argc - 1)
				printf(" ");
		}
	}
	
	return 0;
}
