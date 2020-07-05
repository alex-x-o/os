#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>


#define check_error(expr, msg) \
	do { \
		if(!(expr)){ \
			fprintf(stderr, "%s\n", msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define ARRAY_MAX (256)

typedef struct {
	sem_t inDataReady;
	int array[ARRAY_MAX];
	unsigned arrayLen;
} osData;

void *createMemBlock(const char *path, unsigned size);

int main(int argc, char **argv)
{
	check_error(argc == 2, "argc");
	
	osData *inData = createMemBlock(argv[1], sizeof(osData));
	
	check_error(sem_init(&(inData->inDataReady), 1, 0) != -1, "sem_wait");
	
	inData->arrayLen = 20;
	inData->array[0] = 96;
	inData->array[1] = 12;
	inData->array[2] = 66;
	inData->array[3] = 87;
	inData->array[4] = 30;
	inData->array[5] = 87;
	inData->array[6] = 78;
	inData->array[7] = 69;
	inData->array[8] = 22;
	inData->array[9] = 72;
	inData->array[10] = 27;
	inData->array[11] = 4;
	inData->array[12] = 63;
	inData->array[13] = 89;
	inData->array[14] = 74;
	inData->array[15] = 81;
	inData->array[16] = 27;
	inData->array[17] = 42;
	inData->array[18] = 16;
	inData->array[19] = 88;
	
	check_error(sem_post(&(inData->inDataReady)) != -1, "sem_wait");
	
	check_error(munmap(inData, sizeof(inData)) != -1, "munmap");
	
	//check_error(shm_unlink(argv[1]) != -1, "shm_unkink");
	
	return 0;
}

void *createMemBlock(const char *path, unsigned size)
{
	int memFd = shm_open(path, O_RDWR | O_CREAT, 0600);
	check_error(memFd != -1, "shm_open");
	
	check_error(ftruncate(memFd, size) != -1, "ftruncate");
	
	void *addr;
	check_error((addr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	return addr;
}
