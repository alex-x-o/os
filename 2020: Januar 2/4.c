#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
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
	int epollFd = epoll_create(numFifos);
	check(epollFd != -1, "epoll_create");
	
	int *fds = malloc(numFifos * sizeof(int));
	check(fds != NULL, "malloc");
	
	double *maxs = malloc(numFifos * sizeof(double));
	check(maxs != NULL, "malloc");
	
	struct epoll_event currentEvent;
	for(int i = 0; i < numFifos; i++)
	{
		memset(&currentEvent, 0, sizeof(struct epoll_event));
		
		currentEvent.events = EPOLLIN;
		currentEvent.data.fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		check(currentEvent.data.fd != -1, "open");
		
		fds[i] = currentEvent.data.fd;
		
		check(epoll_ctl(epollFd, EPOLL_CTL_ADD, currentEvent.data.fd, &currentEvent) != -1, "epoll_ctl");
	}
	
	int activeFifos = numFifos;
	struct epoll_event readyFds[MAX_EVENTS];
	
	while(activeFifos)
	{
		int retVal = epoll_wait(epollFd, readyFds, MAX_EVENTS, TIME_OUT);
		check(retVal != -1, "epoll_wait");
		
		for(int i = 0; i < retVal; i++)
		{
			if(readyFds[i].events & EPOLLIN)
			{
				double num;
				double currMax;
				int init = 0;
				FILE *f = fdopen(readyFds[i].data.fd, "r");
				check(f != NULL, "fdopen");
				
				while(fscanf(f, "%lf ", &num) == 1)
				{
					if(!init)
					{
						currMax = num;
						init = 1;
					}
					else
					{
						if(num > currMax)
							currMax = num;
					}
				}
				
				for(int j = 0; j < numFifos; j++)
				{
					if(fds[j] == readyFds[i].data.fd)
						maxs[j] = currMax;
				}
			}
			else if(readyFds[i].events & (EPOLLHUP | EPOLLERR))
			{
				close(readyFds[i].data.fd);
				activeFifos--;
			}
		}
	}
	
	int maxFd = 0;
	double max = maxs[0];
	
	for(int i = 0; i < numFifos; i++)
	{
		if(maxs[i] > max)
		{
			max = maxs[i];
			maxFd = i;
		}
	}
	
	char *line = strrchr(argv[maxFd + 1], '/');
	char *maxName = line != NULL ? line + 1 : argv[maxFd + 1];
	
	printf("%g %s\n", max, maxName);
	
	free(fds);
	free(maxs);
	
	close(epollFd);
	return 0;
}
