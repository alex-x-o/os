#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>

#define check(expr, msg) \
	do{ \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)
	
#define check_pthread(err, msg) \
	do{ \
		int _err = err; \
		if(_err > 0){ \
			errno = _err; \
			check(0, msg); \
		} \
	}while(0)

typedef struct{
	int idx;
}inData;

double p;
int n, m;
double **matrix;
double globalSum = 0;
pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;

void *threadFunc(void *arg);

int main(int argc, char **argv)
{
	check(argc == 1, "argc");
	
	scanf("%lf %d %d", &p, &n, &m);
	
	matrix = malloc(n * sizeof(double*));
	check(matrix != NULL, "malloc");
	int i, j;
	
	for(i = 0; i < n; i++)
	{
		matrix[i] = malloc(m * sizeof(double));
		check(matrix[i] != NULL, "malloc");
	}
	
	for(i = 0; i < n; i++)
		for(j = 0; j < m; j++)
			scanf("%lf", &matrix[i][j]);
	
	pthread_t *tids = malloc(n * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	inData *args = malloc(n * sizeof(inData));
	check(args != NULL, "malloc");
	
	for(i = 0; i < n; i++)
	{
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(i = 0; i < n; i++)
	{
		check_pthread(pthread_join(tids[i], NULL), "pthread_join");
	}
	
	printf("%lf\n", pow(globalSum, 1/p));
	
	return 0;
}

void *threadFunc(void *arg)
{
	inData *data = (inData*)arg;
	int k = data->idx;
	double curr;
	double sum = 0;
	
	for(int i = 0; i < m; i++)
	{
		curr = fabs(matrix[k][i]);
		sum += pow(curr, p);
	}
	
	check(pthread_mutex_lock(&globalLock) != -1, "pthread_mutex_lock");
	
	globalSum += sum;
	
	check(pthread_mutex_unlock(&globalLock) != -1, "pthread_mutex_unlock");
	
	return NULL;
}
