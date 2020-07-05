#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <math.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)

#define ARRAY_MAX (1024)

typedef struct{
	sem_t dataProcessingFinished;
	int array[ARRAY_MAX];
	unsigned arrayLen;
} OsInputData;

int sigUsr1 = 0;
int sigUsr2 = 0;

void *getMemBlock(const char *path, unsigned *size);
void sigHandler(int signum);

int main(int argc, char **argv)
{
	check(argc == 2, "argc");
	
	unsigned size = 0;
	OsInputData *data = getMemBlock(argv[1], &size);
	
	printf("%jd\n", (intmax_t)getpid());
	
	check(signal(SIGUSR1, sigHandler) != SIG_ERR, "signal");
	check(signal(SIGUSR2, sigHandler) != SIG_ERR, "signal");
	
	pause();
	
	int *copy = data->array;
	int n = data->arrayLen;
	
	if(sigUsr1)
	{
		for(int i = 0; i < n; i++)
		{
			if(copy[i] > 0)
				copy[i] = copy[i] - 2 * copy[i];
			else
				copy[i] = abs(copy[i]);
		}
	}
	else if(sigUsr2)
	{
		for(int i = 0; i < n; i++)
		{
			copy[i] = 2 * copy[i];
		}
	}
	for(int i = 0; i < n; i++)
	{
		printf("%d ", copy[i]);
	}
	
	check(sem_post(&(data->dataProcessingFinished)) != -1, "sem_post");
	
	check(munmap(data, size) != -1, "munmap");
	
	return 0;
}

void *getMemBlock(const char *path, unsigned *size)
{
	int memFd = shm_open(path, O_RDWR, 0600);
	check(memFd != -1, "shm_open");
	
	struct stat fInfo;
	check(fstat(memFd, &fInfo) != -1, "fstat");
	*size = fInfo.st_size;
	
	void *addr;
	check((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	return addr;
}

void sigHandler(int signum)
{
	switch(signum){
		case SIGUSR1:
			sigUsr1 = 1;
			break;
		case SIGUSR2:
			sigUsr2 = 1;
			break;
		default:
			check(0, "sig");
	}
}
