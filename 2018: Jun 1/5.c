#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>

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

void *getMemBlock(const char *path, unsigned *size);

int main(int argc, char** argv)
{
	check(argc == 2, "argc");
	
	unsigned size = 0;
	osData *data = getMemBlock(argv[1], &size);
	
	check(sem_wait(&(data->ready)) != -1, "sem_wait");
	
	int n = data->nPoints;
	double *points = data->points;
	double x0, y0, x1, y1;
	double dist;
	double minDist;
	int init = 0;
	int i, j;
	
	for(i = 0; i < n; i+=2)
	{
		x0 = points[i];
		y0 = points[i+1];
		
		for(j = 0; j < n; j+=2)
		{
			if(i == j)
				continue;
			x1 = points[j];
			y1 = points[j+1];
			
			dist = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
			if(!init)
			{
				minDist = dist;
				init = 1;
			}
			else
			{
				if(dist < minDist)
					minDist = dist;
			}
		}
		
	}
	
	check(sem_post(&(data->done)) != -1, "sem_post");
	
	check(munmap(data, size) != -1, "munmap");
	
	printf("%lf\n", minDist);
	
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
