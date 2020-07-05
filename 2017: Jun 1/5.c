#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <math.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define MAX_SIZE (1024)		

typedef struct {
	sem_t inDataReady;
	float array[MAX_SIZE];
	int arrayLen;
} osInputData;

void* osGetMemoryBlock(const char* path, int* size);

int main(int argc, char** argv) {
	
	check(argc == 2, "./5 pathToFile");
	
	int size = 0;
	osInputData* data = osGetMemoryBlock(argv[1], &size);
	
	check(sem_wait(&(data->inDataReady)) != -1, "sem wait failed");
	
	float sum = 0;
	int i = 0;
	for (i = 0; i < data->arrayLen; i++) {
		sum += data->array[i];
	}
	sum /= data->arrayLen;
	
	float stdDev = 0;
	for (i = 0; i < data->arrayLen; i++) 
	{
		stdDev += (data->array[i] - sum)*(data->array[i] - sum);
	}
	stdDev = sqrt(stdDev / data->arrayLen);
	
	printf("%f\n", stdDev);
	
	check(munmap(data, size) != -1, "munmap failed");
	
	exit(EXIT_SUCCESS);
}

void* osGetMemoryBlock(const char* path, int* size) {
	
	int memFd = shm_open(path, O_RDWR, 0600);
	check(memFd != -1, "shmopen failed");
	
	struct stat fInfo;
	check(fstat(memFd, &fInfo) != -1, "stat failed");
	*size = fInfo.st_size;
	
	void* addr;
	check((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap failed");
	
	return addr;
}
