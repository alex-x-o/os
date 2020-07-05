#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
void print(int month);

int main(int argc, char **argv)
{
	check(argc == 3, "argc");
	
	long seconds = atol(argv[1]);
	int numOfDays = atoi(argv[2]);
	
	struct tm *local = localtime(&seconds);
	check(local != NULL, "localtime");
	
	int month = local->tm_mon;
	print(month);
	
	local->tm_mday += numOfDays;
	time_t newTime = mktime(local);
	struct tm *newLocal = localtime(&newTime);
	
	month = newLocal->tm_mon;
	print(month);
	
	return 0;
}

void print(int month)
{
	switch(month){
		case 0:
			printf("januar ");
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
	}
}
