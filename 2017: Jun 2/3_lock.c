#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

int main(int argc, char **argv)
{
	check(argc == 2, "./1 pathToFile");
	
	int fd = open(argv[1], O_RDWR);
	check(fd != -1, "open");
	
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 8;
	lock.l_len = 5;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	sleep(10);
	
	return 0;
}
