#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define check(expr, msg) \
	do { \
		if(!(expr)) { \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)
	
#define check_pthread(err, msg) \
	do { \
		int _err = err; \
		if(_err > 0) { \
			errno = _err; \
			check(0, msg); \
		} \
	} while(0)
	
#define MAX_SIZE (2048)
	
typedef struct {
	char *file;
	int start;
	int end;
} data_t;

int n, M;
int *niz;
int index = 0;
char *stop[3] = {"tag", "taa", "tga"};
pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;

void *threadFunc(void *arg);

int main(int argc, char **argv)
{
	check(argc == 3, "./3 num path");
	
	n = atoi(argv[1]);
	check(n > 0, "poz");
	
	struct stat fInfo;
	check(stat(argv[2], &fInfo) != -1, "stat");
	M = fInfo.st_size;
	
	pthread_t *tids = malloc(n * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	data_t *args = malloc(n * sizeof(data_t));
	check(args != NULL, "malloc");
	
	niz = calloc(MAX_SIZE, sizeof(int));
	check(niz != NULL, "calloc");
	
	for(int i = 0; i < n; i++)
	{
		args[i].file = argv[2];
		args[i].start = M/n * i;
		args[i].end = M/n * i + M/n;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < n; i++)
		check_pthread(pthread_join(tids[i], NULL), "pthread_join");
	
	for(int i = 0; i < MAX_SIZE; i++)
	{
		if(niz[i] == 0)
			break;
		printf("%d ", niz[i]);
	}
	
	free(tids);
	free(args);
	free(niz);
	
	return 0;
}

void *threadFunc(void *arg)
{
	data_t *data = (data_t*)arg;
	
	char *file = data->file;
	int s = data->start;
	int e = data->end;
	printf("%d %d\n", s, e);
	
	FILE *f = fopen(file, "r");
	check(f != NULL, "fopen");
	check(fseek(f, s, SEEK_SET) != -1, "fseek");
	
	int pos[M];
	int m = 0;
	int len = 3;
	
	for(int i = 0; i < e - s - len + 1; i++)
	{
		int ch;
		int newCh;
		int get;
		int chars[MAX_SIZE];
		int count;
		ch = fgetc(f);
		
		if(ch == (int)stop[0][0] || ch == (int)stop[1][0] || ch == (int)stop[2][0])
		{
			get = 0;
			count = 0;
			
			for(int j = 1; j < len; j++)
			{
				newCh = fgetc(f);
				chars[get] = newCh;
				get++;
				if(newCh == (int)stop[0][j] || newCh == (int)stop[1][j] || newCh ==  (int)stop[2][j])
				{
					count++;
				}
				else
					break;
			}
			if(count == (len - 1))
			{
				pos[m] = i + s;
				m++;
			}
			
			while(get != 0)
			{
				get--;
				check(ungetc(chars[get], f) != EOF, "ungetc");
			}
		}
	}
	printf("\n");
	
	check_pthread(pthread_mutex_lock(&globalLock), "lock");
	
	for(int i = 0; i < m; i++)
	{
		niz[index] = pos[i];
		index++;
	}
	
	check_pthread(pthread_mutex_unlock(&globalLock), "unlock");
	
	fclose(f);
	return NULL;
}
