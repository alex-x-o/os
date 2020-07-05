#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
int sigUsr1 = 0;
int sigUsr2 = 0;
int sigTerm = 0;

void sigHandler(int signum);

int main(int argc, char **argv)
{
	check(argc == 1, "argc");
	
	//printf("%jd\n", (intmax_t)getpid());
	
	do{
		check(signal(SIGUSR1, sigHandler) != SIG_ERR, "signal");
		check(signal(SIGUSR2, sigHandler) != SIG_ERR, "signal");
		check(signal(SIGTERM, sigHandler) != SIG_ERR, "signal");
		
		pause();
		
		if(sigTerm)
			break;
		
		int num;
		scanf("%d", &num);
		
		if(sigUsr1)
		{
			sigUsr1 = 0;
			printf("%d\n", abs(num));
		}
		else if(sigUsr2)
		{
			sigUsr2 = 0;
			printf("%g\n", pow(num, 2));
		}
	}while(1);
	
	return 0;
}

void sigHandler(int signum)
{
	switch(signum){
		case SIGUSR1:
			sigUsr1 = 1;
			break;
		case SIGUSR2:
			sigUsr2 = 1;
			break;
		case SIGTERM:
			sigTerm = 1;
			break;
		default:
			exit(EXIT_FAILURE);
	}
}
