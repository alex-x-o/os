#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define ARRAY_MAX (1024)

typedef struct {
	sem_t inDataReady;
	float array[ARRAY_MAX];
	unsigned arrayLen;
} osInputData;

void *getMemoryBlock(char *fpath, unsigned *size);
int cmpFl(const void *a, const void *b);

int main(int argc, char **argv)
{
	check(argc == 2, "Usage: ./5 pathToMem");
	
	unsigned size = 0;
	osInputData *sharedMem = getMemoryBlock(argv[1], &size);
	
	check(sem_wait(&(sharedMem->inDataReady)) != -1, "sem wait");
	
	qsort(&sharedMem->array, sharedMem->arrayLen, sizeof(float), cmpFl);
	
	if(sharedMem->arrayLen % 2 == 1)
		printf("%f\n", sharedMem->array[sharedMem->arrayLen / 2]);
	else
		printf("%f\n", sharedMem->array[sharedMem->arrayLen / 2 - 1]);
	
	check(munmap(sharedMem, size) != -1, "munmap");
	
	return 0;
}

void *getMemoryBlock(char *fpath, unsigned *size)
{
	int memFd = shm_open(fpath, O_RDWR, 0600);
	check(memFd != -1, "shm_open");
	
	struct stat fInfo;
	check(fstat(memFd, &fInfo) != -1, "fstat");
	*size = fInfo.st_size;
	
	void *addr;
	check((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	
	return addr;
}

int cmpFl(const void *a, const void *b)
{
	if(*(float*)a > *(float*)b)
		return -1;
	else if(*(float*)a < *(float*)b)
		return 1;
	else
		return 0;
}
