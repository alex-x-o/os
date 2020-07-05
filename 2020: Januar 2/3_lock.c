#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

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
	
	char *path = argv[1];
	
	FILE *f = fopen(path, "r+");
	check(f != NULL, "fopen");
	
	int fd = fileno(f);
	check(fd != -1, "fileno");
	
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 8;
	lock.l_len = 5;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	sleep(10);
	
	return 0;
}
