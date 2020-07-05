#include <stdio.h>
#include <stdlib.h>

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
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
	sem_t inDAtaReady;
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
	check((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	return addr;
}

int main(int argc, char **argv)
{
	check(argc == 2, "./5 mem");
	
	unsigned size = 0;
	osInputData *data = getMemBlock(argv[1], &size);
	
	check(sem_wait(&(data->inDAtaReady)) != -1, "sem_wait");
	
	for(unsigned i = 0; i < data->arrayLen; i++)
	{
		int num = data->array[i];
		if(num == 1)
			printf("%d ", num);
		else
		{
			int cpy = num;
			int jeste = 1;
			do{
				if(cpy / 3 == 1 && cpy % 3 == 0)
					break;
				if(cpy % 3 != 0)
				{
					jeste = 0;
					break;
				}
				cpy /= 3;
			} while(cpy != 0);
			if(jeste)
				printf("%d ", num);
		}
	}
	
	check(munmap(data, size) != -1, "munmap failed");
	
	return 0;
}
