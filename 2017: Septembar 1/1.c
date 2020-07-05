#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define MAX_SIZE (64)

int _sigusr1 = 0;
int _sigusr2 = 0;
int terminate = 0;

void sigHandler(int signum);

int main(int argc, char **argv)
{
	check(argc == 1, "./1");
	
	//printf("%ld\n", (intmax_t)getpid());
	
	char str[MAX_SIZE];
	
	do
	{
		check(signal(SIGUSR1, sigHandler) != SIG_ERR, "signal");
		check(signal(SIGUSR2, sigHandler) != SIG_ERR, "signal");
		check(signal(SIGTERM, sigHandler) != SIG_ERR, "signal");
		
		pause();
		
		if(terminate)
			break;
		
		scanf("%s", str);
		
		if(_sigusr1)
		{
			_sigusr1 = 0;
			int len = strlen(str);
			int end = len - 1;
			char res[MAX_SIZE];
			
			int i;
			for(i = 0; i < len; i++)
			{
				res[i] = str[end];
				end--;
			}
			res[i] = '\0';
			printf("%s\n", res);
		}
		else if(_sigusr2)
		{
			_sigusr2 = 0;
			int len = strlen(str);
			for(int i = 0; i < len; i++)
			{
				str[i] = toupper(str[i]);
			}
			printf("%s\n", str);
		}
		
	} while(1);
	
	return 0;
}

void sigHandler(int signum)
{
	switch(signum)
	{
		case SIGUSR1:
			_sigusr1 = 1;
			break;
		case SIGUSR2:
			_sigusr2 = 1;
			break;
		case SIGTERM:
			terminate = 1;
			break;
	}
}
