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
	check(argc == 3, "argc");
	
	char *find = argv[2];
	
	FILE *f = fopen(argv[1], "r+");
	check(f != NULL, "fopen");
	
	int fd = fileno(f);
	check(fd != -1, "fileno");
	
	char buffer[MAX_SIZE];
	struct flock lock;
	int len = strlen(find);
	
	while(!feof(f))
	{
		fscanf(f, "%s", buffer);
		//printf("%s\n", buffer);
		
		if(strcmp(buffer, find))
			continue;
		
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ftell(f);
		lock.l_len = -len;
		
		int res = fcntl(fd, F_GETLK, &lock);
		
		if(res == -1)
		{
			if(errno != EACCES && errno != EAGAIN)
				check(0, "fcntl");
		}
		else
		{
			if(lock.l_type == F_RDLCK)
			{
				printf("%ld %c\n", ftell(f) - len, 'r');
			}
			else if(lock.l_type == F_WRLCK)
			{
				printf("%ld %c\n", ftell(f) - len, 'w');
			}
				
		}
	}
	
	fclose(f);
	return 0;
}
