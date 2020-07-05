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
	
#define MAX_SIZE (256)
	
int main(int argc, char **argv)
{
	check(argc == 4, "argc");
	
	char *path = argv[1];
	char *find = argv[2];
	char *replace = argv[3];
	int len = strlen(find);
	
	FILE *f = fopen(path, "r+");
	check(f != NULL, "fopen");
	
	int fd = fileno(f);
	check(fd != -1, "fileno");
	
	struct flock lock;
	char str[MAX_SIZE];
	memset(str, 0, MAX_SIZE);
	int count = 0;
	while(fscanf(f, "%s", str) != EOF)
	{
		if(strcmp(str, find))
			continue;
		
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ftell(f);
		lock.l_len = -len;
		
		int res = fcntl(fd, F_SETLK, &lock);
		if(res == -1)
		{
			if(errno != EACCES && errno != EAGAIN)
				check(0, "fcntl");
			else
				count++;
		}
		else
		{
			check(lseek(fd, -len, SEEK_CUR) != (off_t) - 1, "lseek");
			fprintf(f, "%s", replace);
		}
	}
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	printf("%d\n", count);
	
	fclose(f);
	return 0;
}
