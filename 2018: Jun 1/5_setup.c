#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
#define MAX_SIZE (1024)

typedef struct{
	sem_t ready;
	sem_t done;
	double points[MAX_SIZE];
	unsigned nPoints;
}osData;

void *createMemBlock(const char *path, unsigned size);

int main(int argc, char** argv)
{
	check(argc == 2, "argc");
	
	osData *data = createMemBlock(argv[1], sizeof(osData));
	
	check(sem_init(&(data->ready), 1, 0) != -1, "sem_init");
	check(sem_init(&(data->done), 1, 0) != -1, "sem_init");
	
	data->nPoints = 10;
	data->points[0] = -1.3;
	data->points[1] = 1.7;
	data->points[2] = 15.4;
	data->points[3] = -101.3;
	data->points[4] = -1.4;
	data->points[5] = 1.9;
	data->points[6] = 13.2;
	data->points[7] = -99.5;
	data->points[8] = -17;
	data->points[9] = 0;
	
	check(sem_post(&(data->ready)) != -1, "sem_post");
	
	check(munmap(data, sizeof(osData)) != -1, "munmap");
	
	return 0;
}

void *createMemBlock(const char *path, unsigned size)
{
	int memFd = shm_open(path, O_RDWR | O_CREAT, 0600);
	check(memFd != -1, "shm_open");
	
	check(ftruncate(memFd, size) != -1, "ftruncate");
	
	void *addr;
	check((addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	return addr;
}
