#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <errno.h>

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
	
typedef struct {
	int idx;
} data_t;

int maxGl;
int maxTh;
int init = 0;
int **matrica;
int v;
pthread_mutex_t globalMutex = PTHREAD_MUTEX_INITIALIZER;

void *threadFunc(void *arg)
{
	data_t *data = (data_t*) arg;
	
	int in = 0;
	for(int i = 0; i < v; i++)
		in += matrica[i][data->idx];
	
	check_pthread(pthread_mutex_lock(&globalMutex), "lock");
	
	if(init)
	{
		if(in > maxGl)
		{
			maxGl = in;
			maxTh = data->idx;
		}
	}
	else
	{
		init = 1;
		maxGl = in;
		maxTh = data->idx;
	}
	
	check_pthread(pthread_mutex_unlock(&globalMutex), "unlock");
	
	return NULL;
}

int main(int argc, char **argv)
{
	check(argc == 1, "./2");
	
	scanf("%d", &v);
	
	matrica = malloc(v * sizeof(int*));
	check(matrica != NULL, "malloc");
	
	for(int i = 0; i < v; i++)
	{
		matrica[i] = malloc(v * sizeof(int));
		check(matrica[i] != NULL, "malloc");
	}
	
	for(int i = 0; i < v; i++)
		for(int j = 0; j < v; j++)
			scanf("%d", &matrica[i][j]);
		
	pthread_t *tids = malloc(v * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	data_t *args = malloc(v * sizeof(data_t));
	check(args != NULL, "malloc");
	
	for(int i = 0; i < v; i++)
	{
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < v; i++)
		check_pthread(pthread_join(tids[i], NULL), "pthread_join");
	
	printf("%d", maxTh);
	
	for(int i = 0; i < v; i++)
		free(matrica[i]);
	free(matrica);
	
	free(tids);
	free(args);
	
	return 0;
}
