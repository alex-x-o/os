#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>


#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

int main(int argc, char **argv)
{
	check(argc == 4, "Usage: ./4 pathToFile a b");
	
	int read = 0;
	int write = 0;
	
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = atoi(argv[2]);
	lock.l_len = atoi(argv[3]);
	
	int fd = open(argv[1], O_RDWR);
	check(fd != -1, "open");
	
	check(fcntl(fd, F_GETLK, &lock) != -1, "fcntl");
	
	if(lock.l_type == F_UNLCK)
		read = 1;
	
	lock.l_type = F_WRLCK;
	
	check(fcntl(fd, F_GETLK, &lock) != -1, "fcntl");
	
	if(lock.l_type == F_UNLCK)
		write = 1;
	
	if(read && write)
		printf("unlocked");
	else if(read && !write)
		printf("shared lock");
	else
		printf("exclusive lock");
	
	close(fd);
	
	return 0;
}
