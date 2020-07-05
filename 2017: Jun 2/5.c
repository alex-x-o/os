#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
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

void *getMemBlock(const char *path, unsigned *size)
{
	int memFd = shm_open(path, O_RDWR, 0600);
	check(memFd != -1, "shm_open");
	
	struct stat fInfo;
	check(fstat(memFd, &fInfo) != -1, "fstat");
	*size = fInfo.st_size;
	
	void *addr;
	addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
	check(addr != MAP_FAILED, "mmap failed");
	
	close(memFd);
	return addr;
}

int main(int argc, char **argv)
{
	check(argc == 2, "./1 pathToFile");
	
	unsigned size = 0;
	osInputData *data = getMemBlock(argv[1], &size);
	
	check(sem_wait(&data->inDataReady) != -1, "sem_wait");
	
	for(unsigned i = 0; i < data->arrayLen; i++)
	{
		int num = data->array[i];
		int cnt = 0;
		int cpy = num;
		do 
		{
			if(cpy % 2 == 1)
				cnt++;
			cpy /= 2;
		}while(cpy != 0);
		
		if(cnt >= 4)
			printf("%d ", num);
	}
	
	check(munmap(data, size) != -1, "munmap");
	//check(shm_unlink(argv[1]) != -1, "shm_unlink");
	
	return 0;
}
