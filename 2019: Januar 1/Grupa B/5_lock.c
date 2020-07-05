#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define MAX_SIZE (256)

int main(int argc, char **argv)
{
	check(argc == 2, "argc");
	
	int fd = open(argv[1], O_RDWR);
	check(fd != -1, "open");
	
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 8;
	lock.l_len = 5;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 16;
	lock.l_len = 4;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	sleep(10);
	
	return 0;
}
