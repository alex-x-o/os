#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define MAX_ARRAY 1024	
	
typedef struct {
	sem_t inDataReady;
	float array[MAX_ARRAY];
	unsigned arrayLen;
} OsInputData;

void *osCreateMemoryBlock(char *filePath, unsigned size);

int main(int argc, char** argv) {
	
	check(argc == 2, "argc");
	
	OsInputData* data = osCreateMemoryBlock(argv[1], sizeof(OsInputData));
	
	check(sem_init(&data->inDataReady, 1, 0) != -1, "sem init failed"); 
	
	/*data->arrayLen = 4;
	
	data->array[0] = 7.0;
	data->array[1] = -5.0;
	data->array[2] = 12.0;
	data->array[3] = 10.0;*/
	
	static const float copy[101] = {0.30366, -0.180666, -0.512851, 0.140823, -0.0451198, -0.113494, 0.955261, 0.625765, -0.842486, 0.750642, -0.673716, 0.115484, -0.220804, -0.167328, 0.648994, -0.854865, -0.949426, -0.342866, 0.603929, 0.585569, 0.476604, 0.216474, -0.88679, 0.979524, 0.334522, 0.0679028, 0.646574, 0.754377, -0.328076, 0.602623, 0.313527, 0.222043, 0.247618, -0.965154, -0.124313, -0.335518, 0.446995, -0.595336, 0.473437, -0.0336878, -0.0309474, -0.139799, 0.0701756, -0.601697, 0.0943296, -0.0206527, -0.712087, 0.264565, -0.715761, 0.598781, 0.612813, 0.114967, 0.632172, 0.423928, 0.977107, -0.12855, -0.0599457, 0.618895, 0.573661, -0.627509, 0.516979, -0.630959, -0.107501, 0.866551, 0.346274, 0.341352, -0.763603, 0.263505, -0.914356, -0.75907, 0.778908, 0.92179, 0.754106, -0.0541669, 0.297293, -0.0875372, 0.111199, 0.712676, 0.159726, 0.389361, 0.00959337, -0.275422, 0.369108, -0.00748122, -0.993176, 0.440867, -0.0976957, 0.851378, 0.924051, -0.123569, -0.855491, 0.896022, -0.0346598, 0.722904, 0.661461, -0.657644, -0.723508, -0.321855, 0.324223, -0.158769, 0.851985};
	
	data->arrayLen = 101;
	memcpy(data->array, copy, sizeof(copy));
	
	/*data->arrayLen = 5;
	
	data->array[0] = 1.0;
	data->array[1] = 2.0;
	data->array[2] = 3.0;
	data->array[3] = 4.0;
	data->array[4] = 5.0;*/
	
	check(sem_post(&data->inDataReady) != -1, "sem post failed");
	
	check(munmap(data, sizeof(OsInputData)) != -1, "munmap failed");
	
	exit(EXIT_SUCCESS);
}

void *osCreateMemoryBlock(char* filePath, unsigned size) {
	
	int memFd = shm_open(filePath, O_RDWR | O_CREAT, 0600);
	check(memFd != -1, "shm_open faield");
	
	check(ftruncate(memFd, size) != -1, "ftruncate failed");
	
	void* addr;
	
	check((addr = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap failed");
	
	close(memFd);
	
	return addr;
}
