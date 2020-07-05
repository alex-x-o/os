#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define MAX_SIZE (256)

int main(int argc, char **argv)
{
	check(argc == 2, "Usage: ./4 pathToFile");
	
	int cnt = 0;
	FILE *f = fopen(argv[1], "r");
	check(f != NULL, "fopen");
	
	int fd = fileno(f);
	check(fd != -1, "fileno");
	
	struct flock lock;

	char buffer[MAX_SIZE];
	
	while(!feof(f))
	{
		char *err = buffer;
		memset(buffer, 0, MAX_SIZE);
		fscanf(f, "%s", buffer);
		
		int num = strtol(buffer, &err, 10);
		if(err == buffer || *err != '\0')
			continue;
		
		int len = strlen(buffer);
		
		lock.l_type = F_RDLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ftell(f);
		lock.l_len = -len;
		
		int res = fcntl(fd, F_SETLK, &lock);
		if(res == -1)
		{
			if(errno != EACCES && errno != EAGAIN)
				check(0, "fcntl");
		}
		else
		{
			cnt++;
		}
	}
	
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	check(fcntl(fd, F_SETLK, &lock) != -1, "unlocking failed");
	
	printf("%d\n", cnt);
	
	fclose(f);
	
	return 0;
}
