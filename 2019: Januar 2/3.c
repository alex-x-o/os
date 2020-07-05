#include <stdio.h>
#include <stdlib.h>

#define check_error(expr, msg) \
	do { \
		if(!(expr)){ \
			fprintf(stderr, "%s\n", msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

int main(int argc, char **argv)
{
	
	return 0;
}
