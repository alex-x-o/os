#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>       
#include <unistd.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
#define MAX_EVENTS (8)
#define TIME_OUT (-1)
	
int main(int argc, char **argv)
{
	check(argc > 1, "argc");
	
	int numFifos = argc - 1;
	struct pollfd *fds = malloc(numFifos * sizeof(struct pollfd));
	check(fds != NULL, "malloc");
	
	FILE **streams = malloc(numFifos * sizeof(FILE*));
	check(streams != NULL, "malloc");
	
	for(int i = 0; i < numFifos; i++)
	{
		int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		check(fd != -1, "open");
		
		streams[i] = fdopen(fd, "r");
		check(streams[i] != NULL, "fdopen");
		
		fds[i].fd = fd;
		fds[i].events = POLLIN;
		fds[i].revents = 0;
	}
	
	int activeFifos = numFifos;
	char *maxName;
	double max;
	int init = 0;
	while(activeFifos)
	{
		int retVal = poll(fds, numFifos, -1);
		check(retVal != -1, "poll");
		
		for(int i = 0; i < numFifos; i++)
		{
			if(fds[i].revents & POLLIN)
			{
				double num;
				while(fscanf(streams[i], "%lf", &num) == 1)
				{
					if(!init)
					{
						init = 1;
						maxName = argv[i+1];
						max = num;
					}
					else
					{
						if(num > max)
						{
							max = num;
							maxName = argv[i+1];
						}
					}
				}
				
				fds[i].revents = 0;
			}
			else if(fds[i].revents & (POLLHUP | POLLERR))
			{
				close(fds[i].fd);
				fds[i].fd = -1;
				fds[i].events = 0;
				fds[i].revents = 0;
				activeFifos--;
			}
		}
	}
	
	char *line = strrchr(maxName, '/');
	char *res = line != NULL ? line + 1 : maxName;
	printf("%g %s", max, res);
	
	free(streams);
	free(fds);
	return 0;
}
