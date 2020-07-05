#include <stdio.h>
#include <stdlib.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
int main(int argc, char **argv)
{
	check(argc > 1, "argc");
	
	
	return 0;
}
