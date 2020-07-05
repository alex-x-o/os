#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define ARRAY_MAX (1024)

typedef struct {
	sem_t inDataReady;
	int array[ARRAY_MAX];
	unsigned arrayLen;
} osInputData;

void *createMemBlock(const char *path, unsigned size)
{
	int memFd = shm_open(path, O_RDWR | O_CREAT, 0600);
	check(memFd != -1, "shm_open");
	
	check(ftruncate(memFd, size) != -1, "ftruncate");
	
	void *addr;
	addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
	check(addr != MAP_FAILED, "mmap failed");
	
	close(memFd);
	return addr;
}

int main(int argc, char **argv)
{
	check(argc == 2, "./1 pathToFile");
	
	osInputData *data = createMemBlock(argv[1], sizeof(osInputData));
	
	check(sem_init(&data->inDataReady, 1, 0) != -1, "sem_init");
	
	data->arrayLen = 6;
	data->array[0] = 3;
	data->array[1] = 1;
	data->array[2] = 2;
	data->array[3] = 15;
	data->array[4] = 29;
	data->array[5] = 63;
	
	check(sem_post(&data->inDataReady) != -1, "sem_post");
	
	check(munmap(data, sizeof(data)) != -1, "munmap failed");
	
	return 0;
}
