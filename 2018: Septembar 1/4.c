#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)

#define MAX_SIZE (128)

int main(int argc, char **argv)
{
	check(argc > 1, "argc");
	
	int numFifos = argc - 1;
	struct pollfd *fifos = calloc(numFifos, sizeof(struct pollfd));
	check(fifos != NULL, "malloc");
	FILE **streams = malloc(numFifos * sizeof(FILE*));
	check(streams != NULL, "malloc");
	
	for(int i = 0; i < numFifos; i++)
	{
		int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
		check(fd != -1, "open");
		
		fifos[i].fd = fd;
		fifos[i].events = POLLIN | POLLERR | POLLHUP;
		fifos[i].revents = 0;
		
		streams[i] = fdopen(fd, "r");
		check(streams[i] != NULL, "fdopen");
	}
	
	char name[MAX_SIZE];
	int max = 0;
	int activeFifos = numFifos;
	
	while(activeFifos)
	{
		int retVal = poll(fifos, numFifos, -1);
		check(retVal != -1, "poll");
		
		for(int i = 0; i < numFifos; i++)
		{
			if(fifos[i].revents & POLLIN)
			{
				int num = 0;
				char *line = NULL;
				size_t lineLen = 0;
				
				while(getline(&line, &lineLen, streams[i]) != -1)
				{
					int len = (int)lineLen;
					for(int i = 0; i < len; i++)
					{
						if(line[i] == 'a')
							num++;
					}
				}
				
				if(num > max)
				{
					max = num;
					strcpy(name, argv[i+1]);
				}
				fifos[i].revents = 0;
			}
			else if(fifos[i].revents & (POLLHUP | POLLERR))
			{
				fclose(streams[i]);
				close(fifos[i].fd);
				fifos[i].fd = -1;
				fifos[i].events = 0;
				fifos[i].revents = 0;
				
				activeFifos--;
			}
		}
	}
	
	char *line = strrchr(name, '/');
	char *res = line != NULL ? line + 1 : name;
	printf("%s %d", res, max);
	
	free(fifos);
	free(streams);
	
	return 0;
}
