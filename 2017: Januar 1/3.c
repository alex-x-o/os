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
	
typedef struct{
	int idx;
} data_t;

int m, n;
double **matrica;
int init = 0;
double globalMin;
pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;

void *threadFunc(void * arg)
{
	data_t *data = (data_t *)arg;
	
	double localMin = matrica[data->idx][0];
	for(int i = 0; i < m; i++)
		if(matrica[data->idx][i] < localMin)
			localMin = matrica[data->idx][i];
		
	check_pthread(pthread_mutex_lock(&globalLock), "lock");
	
	if(init)
	{
		if(localMin < globalMin)
		{
			globalMin = localMin;
		}
	}
	else
	{
		globalMin = localMin;
		init = 1;
	}
	
	check_pthread(pthread_mutex_unlock(&globalLock), "unlock");
	
	return NULL;
}

int main(int argc, char **argv)
{
	check(argc == 2, "Usage: ./3 pathToFile");
	
	FILE *f = fopen(argv[1], "r");
	check(f != NULL, "fopen");

	fscanf(f, "%d %d", &n, &m);
	matrica = malloc(n * sizeof(double*));
	check(matrica != NULL, "malloc");
	
	for(int i = 0; i < n; i++)
	{
		matrica[i] = malloc(m * sizeof(double));
		check(matrica[i] != NULL, "malloc");
	}
	
	for(int i = 0; i < n; i++)
		for(int j = 0; j < m; j++)
			fscanf(f, "%lf", &matrica[i][j]);
		
	fclose(f);
	
	pthread_t *tids = malloc(sizeof(pthread_t) * n);
	check(tids != NULL, "malloc");
	
	data_t *args = malloc(sizeof(data_t) * n);
	check(args != NULL, "malloc");
	
	for(int i = 0; i < n; i++)
	{
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < n; i++)
	{
		check_pthread(pthread_join(tids[i], NULL), "pthread_join");
	}
	
	printf("%lf", globalMin);
	
	for(int i = 0; i < n; i++)
		free(matrica[i]);
	free(matrica);
	free(tids);
	free(args);
	
	return 0;
}
