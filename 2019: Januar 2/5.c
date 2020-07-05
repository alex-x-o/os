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

void *getMemBlock(const char *path, unsigned *size);

int main(int argc, char **argv)
{
	check_error(argc == 3, "argc");
	
	unsigned sizeIn = 0;
	unsigned sizeOut = 0;
	osData *inData = getMemBlock(argv[1], &sizeIn);
	osData *outData = getMemBlock(argv[2], &sizeOut);
	
	check_error(sem_wait(&(inData->inDataReady)) != -1, "sem_wait");
	
	int *out = outData->array;
	int *in = inData->array;
	int n = (int)inData->arrayLen;
	int j = 0;
	
	for(int i = 0; i < n; i++)
	{
		int current = in[i];
		int original = current;
		int ones = 0;
		
		do
		{
			if(current % 2 == 1)
				ones++;
			current /= 2;
		}while(current != 0);
		
		if(ones >= 4)
		{
			out[j] = original;
			j++;
		}
	}
	
	outData->arrayLen = j;
	/*printf("%d\n", j);
	for(int i = 0; i < j; i++)
	{
		printf("%d ", out[i]);
	}*/
	
	check_error(sem_post(&(outData->inDataReady)) != -1, "sem_post");
	
	check_error(munmap(inData, sizeIn) != -1, "munmap");
	check_error(munmap(outData, sizeOut) != -1, "munmap");
	
	//check_error(shm_unlink(argv[1]) != -1, "shm_unkink");
	
	return 0;
}

void *getMemBlock(const char *path, unsigned *size)
{
	int memFd = shm_open(path, O_RDWR, 0600);
	check_error(memFd != -1, "shm_open");
	
	struct stat fInfo;
	check_error(fstat(memFd, &fInfo) != -1, "fstat");
	*size = fInfo.st_size;
	
	void *addr;
	check_error((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	return addr;
}
