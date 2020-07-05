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
	
int yes = 0;
int no = 0;

int main(int argc, char** argv)
{
	check(argc == 2, "argc");
	
	FILE *f = fopen(argv[1], "r+");
	check(f != NULL, "fopen");
	
	int fd = fileno(f);
	check(fd != -1, "fileno");
	
	struct flock lock;
	char read[MAX_SIZE];
	memset(read, 0, MAX_SIZE);
	
	while(fscanf(f, "%s", read) != EOF)
	{
		int len = strlen(read);
		int end = len-1;
		char copy[MAX_SIZE];
		
		for(int i = 0; i < len; i++)
		{
			copy[i] = read[end];
			end--;
		}
		copy[len] = '\0';
		
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = ftell(f);
		lock.l_len = -len;
		
		int res = fcntl(fd, F_SETLK, &lock);
		if(res == -1)
		{
			no++;
			if(errno != EACCES && errno != EAGAIN)
				check(0, "fcntl");
		}
		else
		{
			check(fseek(f, -len, SEEK_CUR) != -1, "fseek");
			fprintf(f, "%s", copy);
			yes++;
		}
	}
	
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	
	check(fcntl(fd, F_SETLK, &lock) != -1, "fcntl");
	
	printf("%d %d", yes, no);
	fclose(f);
	
	return 0;
}
