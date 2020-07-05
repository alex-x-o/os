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
	check(argc == 2, "./4 num");
	
	long sec = atoi(argv[1]);
		
	struct tm *local = localtime(&sec);
	int month = local->tm_mon;
	
	switch(month) {
		case 0:
			printf("januar");
			break;
		case 1:
			printf("februar");
			break;
		case 2:
			printf("mart");
			break;
		case 3:
			printf("april");
			break;
		case 4:
			printf("maj");
			break;
		case 5:
			printf("jun");
			break;
		case 6:
			printf("jul");
			break;
		case 7:
			printf("avgust");
			break;
		case 8:
			printf("septembar");
			break;
		case 9:
			printf("oktobar");
			break;
		case 10:
			printf("novembar");
			break;
		case 11:
			printf("decembar");
			break;
		default:
			check(0, "switch");
			break;
	}
	
	return 0;
}
