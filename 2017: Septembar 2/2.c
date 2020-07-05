#define _XOPEN_SOURCE 700
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
	
	FILE *f = fopen(argv[1], "r+");
	check(f != NULL, "fopen");
	
	int fd = fileno(f);
	check(fd != -1, "fileno");
	
	struct flock lock;
	char buffer[MAX_SIZE];
	
	while(!feof(f))
	{
		char *str = buffer;
		memset(buffer, 0, MAX_SIZE);
		fscanf(f, "%s", buffer);
		if(*buffer == '-')
			continue;
		
		int num = strtol(buffer, &str, 10);
		if(str == buffer || *str != '\0')
			continue;
		
		int copy = num;
		int numOfDig = 0;
		int len = strlen(buffer);

		do
		{
			copy /= 10;
			numOfDig++;
		}while(copy != 0);
		
		if(numOfDig == 4)
		{
			lock.l_type = F_WRLCK;
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
				check(fseek(f, -len, SEEK_CUR) != -1, "fseek");
				fprintf(f, "%s", "####");
			}
		}
	}
	
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	fclose(f);
	
	return 0;
}
