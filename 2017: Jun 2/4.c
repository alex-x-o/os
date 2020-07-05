#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
int main(int argc, char **argv)
{
	check(argc > 2, "./1 pathToFifo pathToFifo ...");
	
	int numFifos = argc - 1;
	struct pollfd *fileMonitors = calloc(numFifos, sizeof(struct pollfd));
	check(fileMonitors != NULL, "calloc");
	FILE **streams = malloc(numFifos * sizeof(FILE*));
	check(streams != NULL, "malloc");
	
	for(int i = 0; i < numFifos; i++)
	{
		fileMonitors[i].fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		check(fileMonitors[i].fd != -1, "open");
		
		fileMonitors[i].events = POLLIN | POLLHUP | POLLERR;
		
		streams[i] = fdopen(fileMonitors[i].fd, "r");
		check(streams[i] != NULL ,"fdopen");
	}
	
	char *file = NULL;
	int read_num;
	int max_num;
	int init = 0;
	int numActive = numFifos;
	while(numActive)
	{
		int events = poll(fileMonitors, numFifos, -1);
		check(events != -1, "poll");
		
		for(int i = 0; i < numFifos; i++)
		{
			if(fileMonitors[i].revents & POLLIN)
			{
				while(fscanf(streams[i], "%d", &read_num) == 1)
				{
					if(init)
					{
						if(read_num > max_num)
						{
							max_num = read_num;
							file = argv[i + 1];
						}
					}
					else
					{
						max_num = read_num;
						file = argv[i + 1];
						init = 1;
					}
				}
				
				fileMonitors[i].revents = 0;
			}
			else if(fileMonitors[i].revents & (POLLHUP | POLLERR))
			{
				fclose(streams[i]);
				fileMonitors[i].fd = -1;
				fileMonitors[i].events = 0;
				fileMonitors[i].revents = 0;
				numActive--;
			}
		}
	}
	
	char *name = strrchr(file, '/');
	printf("%d %s", max_num, name != NULL ? (name + 1) : file);
	
	free(fileMonitors);
	free(streams);
	
	return 0;
}
