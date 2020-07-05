#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <string.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)

#define ARRAY_MAX (256)

typedef struct{
	sem_t inDataReady;
	sem_t dataProcessed;
	char str[ARRAY_MAX];
}osInputData;

void *createMemoryBlock(const char *path, unsigned size);

int main(int argc, char **argv)
{
	check(argc == 2, "argc");
	
	osInputData *data = createMemoryBlock(argv[1], sizeof(osInputData));
	
	check(sem_init(&(data->inDataReady), 1, 0) != -1, "sem_init");
	check(sem_init(&(data->dataProcessed), 1, 0) != -1, "sem_init");
	
	strcpy(data->str, "Bla_$#!TraLa??Buc");
	
	check(sem_post(&(data->inDataReady)) != -1, "sem_post");
	
	check(munmap(data, sizeof(osInputData)) != -1, "munmap");
	
	return 0;
}

void *createMemoryBlock(const char *path, unsigned size)
{
	int memFd = shm_open(path, O_RDWR | O_CREAT, 0600);
	check(memFd != -1, "shm_open");
	
	check(ftruncate(memFd, size) != -1, "ftruncate");
	
	void *addr;
	check((addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	return addr;
}
