#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define check_error(expr, msg) \
	do { \
		if(!(expr)){ \
			fprintf(stderr, "%s\n", msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

int main(int argc, char **argv)
{
	check_error(argc == 2, "argc");
	
	long seconds = atol(argv[1]);
	
	struct tm *local = localtime(&seconds);
	int hour = local->tm_hour;
	
	if(hour >= 0 && hour < 7)
		printf("noc\n");
	else if(hour >= 7 && hour < 9)
		printf("jutro\n");
	else if(hour >= 9 && hour < 12)
		printf("prepodne\n");
	else if(hour >= 12 && hour < 19)
		printf("popodne\n");
	else if(hour >= 19 && hour <= 23)
		printf("vece\n");
	
	return 0;
}
