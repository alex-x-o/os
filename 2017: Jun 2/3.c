#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define MAX_SIZE (256)

int main(int argc, char **argv)
{
	check(argc == 4, "./3 pathToFile str1 str2");
	
	FILE *f = fopen(argv[1], "r+");
	check(f != NULL, "fopen");
	
	int fd = fileno(f);
	check(fd != -1, "open");
	
	struct flock lock;
	char buf[MAX_SIZE];
	unsigned int cnt = 0;
	while(!feof(f))
	{
		memset(buf, 0, MAX_SIZE);
		fscanf(f, "%s", buf);
		int len = strlen(buf);
		
		if(strcmp(argv[2], buf))
			continue;
		
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ftell(f);
		lock.l_len = -len;
		
		int res = fcntl(fd, F_SETLK, &lock);
		if(res == -1)
		{
			if (errno != EACCES && errno != EAGAIN)
				check(0, "lock failed");
			cnt++;
		}
		else
		{
			check(fseek(f, -len, SEEK_CUR) != -1, "lseek failed");
			fprintf(f, "%s", argv[3]);
		}
	}
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	printf("%u", cnt);
	
	fclose(f);
	
	return 0;
}
