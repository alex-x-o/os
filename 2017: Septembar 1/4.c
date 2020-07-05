#include <stdio.h>
#include <stdlib.h>

#include <time.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

int main(int argc, char **argv)
{
	check(argc == 2, "./2 sec");
	
	long sec = atol(argv[1]);
	
	struct tm *local = localtime(&sec);
	int weekDay = local->tm_wday;
	
	switch(weekDay) {
		case 0:
			printf("nedelja");
			break;
		case 1:
			printf("ponedeljak");
			break;
		case 2:
			printf("utorak");
			break;
		case 3:
			printf("sreda");
			break;
		case 4:
			printf("cetvrtak");
			break;
		case 5:
			printf("petak");
			break;
		case 6:
			printf("subota");
			break;
		default:
			check(0, "switch");
			break;
	}
	
	return 0;
}
