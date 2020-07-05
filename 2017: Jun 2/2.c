#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define RD_END (0)
#define WR_END (1)

int main(int argc, char **argv)
{
	check(argc == 2, "./1 pathToFile");
	
	int cld2par[2];
	check(pipe(cld2par) != -1, "pipe");
	
	int childPid = fork();
	check(childPid != -1, "fork");
	
	char *ext;
	ext = strrchr(argv[1], '.');
	check(ext != NULL, "strrchr");
	strncat(ext, "\n", 1);
	
	int cnt = 0;
	
	if(childPid > 0) //parent
	{
		close(cld2par[WR_END]);
		
		FILE *f = fdopen(cld2par[RD_END], "r");
		check(f != NULL, "fdopen");
		
		size_t lineLen = 0;
		char *line = NULL;
		
		while(getline(&line, &lineLen, f) != -1)
		{
			char *sub;
			sub = strrchr(line, '.');
			
			if(!strcmp(sub, ext))
				cnt++;
		}
		
		printf("%d", cnt);
		
		free(line);
		fclose(f);
		
		close(cld2par[RD_END]);
	}
	else //child
	{
		close(cld2par[RD_END]);
		
		check(dup2(cld2par[WR_END], STDOUT_FILENO) != -1, "dup2");
		
		check(execlp("find", "find", ".", "-type", "f", NULL) != -1, "execlp");
		
		close(cld2par[WR_END]);
		
		exit(EXIT_SUCCESS);
	}
	
	int status = 0;
	check(wait(&status) != -1, "wait");
	
	return 0;
}
