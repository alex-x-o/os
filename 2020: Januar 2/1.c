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
	
int main(int argc, char **argv)
{
	check(argc == 2, "argc");
	
	long seconds = atol(argv[1]);
	
	struct tm *structTime = localtime(&seconds);
	check(localtime != NULL, "localtime");
	
	int year = structTime->tm_year;
	
	for(int i = year; i > 0; i--)
	{
		if((i % 4 == 0 && i % 100 != 0) || (i % 100 == 0 && i % 400 == 0))
		{
			printf("%d\n", i + 1900);
			break;
		}
	}
	
	return 0;
}
