#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
#define MAX_EVENTS (128)
#define MAX_SIZE (256)

int main(int argc, char **argv)
{
	check(argc > 1, "argc");
	
	int numFifos = argc - 1;
	int epollFd = epoll_create(numFifos);
	check(epollFd != -1, "epoll_create");
	
	struct epoll_event currentEvent;
	for(int i = 0; i < numFifos; i++)
	{
		memset(&currentEvent, 0, sizeof(struct epoll_event));
		
		currentEvent.events = EPOLLIN;
		currentEvent.data.fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		check(currentEvent.data.fd != -1, "open");
		check(epoll_ctl(epollFd, EPOLL_CTL_ADD, currentEvent.data.fd, &currentEvent) != -1, "epoll_ctl");
	}
	
	int readBytes;
	char buffer[MAX_SIZE];
	int globalMin = 0;
	char *globalName;
	int init = 0;
	
	struct epoll_event readyFds[MAX_EVENTS];
	while(numFifos)
	{
		int numReadyEvents;
		check((numReadyEvents = epoll_wait(epollFd, readyFds, MAX_EVENTS, -1)) != -1, "epoll_wait");
		
		for(int i = 0; i < numReadyEvents; i++)
		{
			if(readyFds[i].events & EPOLLIN)
			{
				int totalBytes = 0;
				while((readBytes = read(readyFds[i].data.fd, buffer, MAX_SIZE)) > 0)
					totalBytes += readBytes;
				
				check(readBytes != -1, "read");
				buffer[totalBytes] = 0;
				
				if(!init)
				{
					globalMin = totalBytes;
					globalName = argv[i+1];
					init = 1;
				}
				else
				{
					if(totalBytes < globalMin)
					{
						globalMin = totalBytes;
						globalName = argv[i+1];
					}
				}
			}
			else if (readyFds[i].events & (EPOLLHUP | EPOLLERR))
			{
				close(readyFds[i].data.fd);
				numFifos--;
			}
		}
	}
	
	char *line = strrchr(globalName, '/');
	char *res = line != NULL ? line + 1 : globalName;
	printf("%s\n", res);
	
	close(epollFd);
	
	return 0;
}
