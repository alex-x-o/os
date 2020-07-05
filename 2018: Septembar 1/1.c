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
	
#define MAX_SIZE (256)
#define SECONDS (60)

int main(int argc, char **argv)
{
	check(argc == 3, "argc");
	
	long seconds = atol(argv[1]);
	int minutes = atoi(argv[2]);
	seconds += (long)(minutes * SECONDS);
	
	struct tm *time = localtime(&seconds);
	check(time != NULL, "localtime");
	
	char str[MAX_SIZE];
	check(strftime(str, MAX_SIZE, "%d/%m/%Y %R", time) != 0, "strftime");
	
	printf("%s\n", str);
	
	return 0;
}
