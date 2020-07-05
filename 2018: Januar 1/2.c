#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <stdatomic.h>
#include <fcntl.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
#define check_thread(err, msg) \
	do{ \
		int _err = err; \
		if(_err > 0){ \
			errno = _err; \
			check(0, msg); \
		} \
	}while(0)

#define MAX_SIZE (256)
	
typedef struct yes{
	char *file;
	int start;
	int end;
}inData;

char *W;
int K, N;
atomic_int globalSum;

void *threadFunc(void *arg);

int main(int argc, char **argv)
{
	check(argc == 4, "argc");
	
	atomic_init(&globalSum, 0);
	
	W = argv[2];
	K = atoi(argv[3]);
	
	struct stat fInfo;
	check(stat(argv[1], &fInfo) != -1, "stat");
	N = fInfo.st_size;
	
	pthread_t *tids = malloc(K * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	inData *args = malloc(K * sizeof(inData));
	check(args != NULL, "malloc");
	
	for(int i = 0; i < K; i++)
	{
		args[i].file = argv[1];
		args[i].start = i * N/K;
		args[i].end = i * N/K + N/K;
		check_thread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < K; i++)
	{
		check_thread(pthread_join(tids[i], NULL), "pthread_join");
	}
	
	printf("%d\n", atomic_load(&globalSum));
		
	return 0;
}

void *threadFunc(void *arg)
{
	inData *data = (inData*)arg;
	char *file = data->file;
	int s = data->start;
	int e = data->end;
	int count = 0;
	
	FILE *f = fopen(file, "r");
	check(f != NULL, "fopen");
	check(fseek(f, s, SEEK_SET) != -1, "fseek");
	
	for(int i = 0; i < e - s; i++)
	{
		int ch = fgetc(f);
		int k;
		int len = strlen(W);
		int sameChar;
		int newCh;
		int all[len];

		if(ch == (int)W[0])
		{
			k = 0;
			sameChar = 0;
			for(int j = 1; j < len; j++)
			{
				newCh = fgetc(f);
				all[k] = newCh;
				k++;
				if(newCh == (int)W[j])
					sameChar++;
				else
					break;
			}
			if(sameChar == (len - 1))
				count++;
			while(k != 0)
			{
				k--;
				check(ungetc(all[k], f) != EOF, "ungetc");
			}
		}
	}
	
	atomic_fetch_add(&globalSum, count);
	fclose(f);
	
	return NULL;
}
