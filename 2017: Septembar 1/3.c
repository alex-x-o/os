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
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define MAX_EVENTS (8)
#define WAIT_TIME (-1)
#define MAX_SIZE (256)

int main(int argc, char **argv)
{
	check(argc >= 3 , "./3 fifo1 fifo2 ...");
	
	int numFifos = argc - 1;
	int epollFd = epoll_create(numFifos);
	check(epollFd != -1, "epoll_create");
	
	struct epoll_event *events = malloc(numFifos * sizeof(struct epoll_event));
	check(events != NULL, "malloc");
	
	int *fds = malloc(numFifos * sizeof(int));
	check(fds != NULL, "malloc");
	double *sums = malloc(numFifos * sizeof(double));
	check(sums != NULL, "malloc");
	
	for(int i = 0; i < numFifos; i++)
	{
		int fd = open(argv[i+1], O_RDONLY|O_NONBLOCK);
		check(fd != -1, "open");
		
		events[i].events = EPOLLIN | EPOLLHUP | EPOLLERR;
		events[i].data.fd = fd;
		
		fds[i] = events[i].data.fd;
		
		check(epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &events[i]) != -1, "epoll_ctl");
	}
	
	struct epoll_event readyFds[MAX_EVENTS];
	int activeFifos = numFifos;
	while(activeFifos)
	{
		int numReadyEvents;
		check((numReadyEvents = epoll_wait(epollFd, readyFds, MAX_EVENTS, WAIT_TIME)) != -1, "epoll_wait");
		
		for(int i = 0; i < numReadyEvents; i++)
		{
			if(readyFds[i].events & EPOLLIN)
			{
				double num;
				double sum = 0;
				FILE *f = fdopen(readyFds[i].data.fd, "r");
				check(f != NULL, "fdopen");
				
				while(fscanf(f, "%lf", &num) == 1)
				{
					sum+= num;
				}
				
				for(int j = 0 ; j < numFifos; j++)
				{
					if(fds[j] == readyFds[i].data.fd)
					{
						sums[j] = sum;
					}
				}
			}
			else if(readyFds[i].events & (EPOLLHUP|EPOLLERR))
			{
				close(readyFds[i].data.fd);
				activeFifos--;
			}
		}
	}
	
	double max_sum = sums[0];
	int k = 0;
	
	for(int i = 0; i < numFifos; i++)
	{
		if(sums[i] > max_sum)
		{
			k = i;
			max_sum = sums[i];
		}
	}
	
	char *line = strrchr(argv[k + 1], '/');
	char *path = line != NULL ? line + 1 : argv[k + 1];
	
	printf("%s\n", path);
	
	close(epollFd);
	free(events);
	
	return 0;
}
