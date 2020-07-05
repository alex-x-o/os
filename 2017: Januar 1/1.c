#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

#include <signal.h>
#include <stdint.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
int sigusr1Caught = 0;
int sigusr2Caught = 0;
int sigtermCaught = 0;

void sig_handler(int signum)
{
	switch(signum){
		case SIGUSR1:
			sigusr1Caught = 1;
			break;
		case SIGUSR2:
			sigusr2Caught = 1;
			break;
		case SIGTERM:
			sigtermCaught = 1;
			break;
	}
}

int main(int argc, char **argv)
{
	check(argc == 1, "Usage: ./1");
	
	int numOfUsr1 = 0;
	int numOfUsr2 = 0;
	
	//printf("PID: %jd\n", (intmax_t)getpid());
	do {
		check(signal(SIGUSR1, sig_handler) != SIG_ERR, "signal usr1");
		check(signal(SIGUSR2, sig_handler) != SIG_ERR, "signal usr2");
		check(signal(SIGTERM, sig_handler) != SIG_ERR, "signal term");
		
		pause();
		
		if(sigusr1Caught) {
			sigusr1Caught = 0;
			numOfUsr1++;
		}
		if(sigusr2Caught) {
			sigusr2Caught = 0;
			numOfUsr2++;
		}
	} while(!sigtermCaught);
	
	printf("%d %d", numOfUsr1, numOfUsr2);
	
	return 0;
}
