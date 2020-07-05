#include <stdio.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

void sigHandler(int signum);

int sigUsr1 = 0;
int sigUsr2 = 0;
int sigInt = 0;
int sigTerm = 0;

int main(int argc, char **argv)
{
	check(argc == 1, "argc");
	
	int result = 0;
	//printf("%jd\n", (intmax_t)getpid());
	
	do{
		check(signal(SIGUSR1, sigHandler) != SIG_ERR, "signal");
		check(signal(SIGUSR2, sigHandler) != SIG_ERR, "signal");
		check(signal(SIGINT, sigHandler) != SIG_ERR, "signal");
		check(signal(SIGTERM, sigHandler) != SIG_ERR, "signal");
		
		pause();
		
		if(sigUsr1)
		{
			sigUsr1 = 0;
			result += 1;
			
		}
		else if(sigUsr2)
		{
			sigUsr2 = 0;
			result += 2;
		}
		else if(sigInt)
		{
			sigInt = 0;
			result -= 4;
		}
		
	}while(!sigTerm);
	
	printf("%d\n", result);
	
	return 0;
}

void sigHandler(int signum)
{
	switch(signum)
	{
		case SIGUSR1:
			sigUsr1 = 1;
			break;
		case SIGUSR2:
			sigUsr2 = 1;
			break;
		case SIGINT:
			sigInt = 1;
			break;
		case SIGTERM:
			sigTerm = 1;
			break;
		default:
			exit(EXIT_FAILURE);
	}
}
