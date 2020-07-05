#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)

#define MAX_SIZE (1024)

int main(int argc, char **argv)
{
	check(argc > 1, "argc");
	
	int nFds = argc - 1;
	struct pollfd *fds = malloc(nFds * sizeof(struct pollfd));
	check(fds != NULL, "malloc");
	int *count = calloc(nFds, sizeof(int));
	check(count != NULL, "calloc");
	
	for(int i = 0; i < nFds; i++)
	{
		int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		check(fd != -1, "open");
		count[i] = 0;
		
		fds[i].fd = fd;
		fds[i].events = POLLIN | POLLHUP | POLLERR;
		fds[i].revents = 0;
	}
	
	int activeFds = nFds;
	char buffer[MAX_SIZE];
	int globalMax = 0;
	char *globalName;
	
	while(activeFds)
	{
		int retVal = poll(fds, nFds, -1);
		check(retVal != -1, "poll");
		
		for(int i = 0; i < nFds; i++)
		{
			if(fds[i].revents & POLLIN)
			{
				int readBytes = 0;
				while((readBytes = read(fds[i].fd, buffer, MAX_SIZE)) > 0)
				{
					count[i] += readBytes;
				}
				if(readBytes == -1)
					if(errno != EAGAIN)
						check(0, "read");
				
				buffer[totalBytes] = '\0';
				int localMax = totalBytes;
				
				if(localMax > globalMax)
				{
					globalMax = localMax;
					globalName = argv[i+1];
				}
				
				fds[i].revents = 0;
			}
			else if(fds[i].revents & (POLLERR | POLLHUP))
			{
				close(fds[i].fd);
				fds[i].fd = -1;
				fds[i].events = 0;
				fds[i].revents = 0;
				
				activeFds--;
			}
		}
	}
	
	char *line = strrchr(globalName, '/');
	char *res = line != NULL ? line + 1 : globalName;
	printf("%s\n", res);
	
	for(int i = 0; i < nFds; i++)
		if(fds[i].fd != -1)
			close(fds[i].fd);
	free(fds);
	free(count);
	
	return 0;
}
