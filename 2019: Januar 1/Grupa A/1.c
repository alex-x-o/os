#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
void print(int weekDay);

int main(int argc, char **argv)
{
	check(argc == 3, "argc");
	
	long seconds = atol(argv[1]);
	int year = atoi(argv[2]);
	
	struct tm *timeStruct = localtime(&seconds);
	check(timeStruct != NULL, "localtime");
	
	int dayOfTheWeek = timeStruct->tm_wday;
	print(dayOfTheWeek);
	
	timeStruct->tm_year += year;
	time_t newTime = mktime(timeStruct);
	check(newTime != (time_t) - 1, "mktime");
	
	struct tm *newTimeStruct = localtime(&newTime);
	check(newTimeStruct != NULL, "localtime");
	
	dayOfTheWeek = newTimeStruct->tm_wday;
	printf(" ");
	print(dayOfTheWeek);
	
	return 0;
}

void print(int weekDay)
{
	switch(weekDay){
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
	}
}
