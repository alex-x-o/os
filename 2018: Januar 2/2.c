#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>

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

int N = 0;
double **matrix;
double globalMin = 0;
int init = 0;
pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;

void *threadFunc(void *arg);

int main(int argc, char **argv)
{
	check(argc == 1, "argc");
	
	double x, y;
	int alloc = 3;
	matrix = malloc(alloc * sizeof(*matrix));
	check(matrix != NULL, "malloc");
	
	while(scanf("%lf %lf", &x, &y) != EOF)
	{
		if(N == alloc)
		{
			alloc += 5;
			matrix = realloc(matrix, alloc * sizeof(*matrix));
			check(matrix != NULL, "realloc");
		}
		
		matrix[N] = malloc(2 * sizeof(matrix));
		check(matrix[N] != NULL, "malloc");
		
		matrix[N][0] = x;
		matrix[N][1] = y;
		
		N++;
	}
	/*printf("%d\n", N);
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < 2; j++)
			printf("%lf ", matrix[i][j]);
		printf("\n");
	}*/
	
	pthread_t *tids = malloc(N * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	inData *args = malloc(N * sizeof(inData));
	check(args != NULL, "malloc");
	
	for(int i = 0; i < N; i++)
	{
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < N; i++)
	{
		check_pthread(pthread_join(tids[i], NULL), "pthread_join");
	}
	
	printf("%lf\n", globalMin);
	
	check_pthread(pthread_mutex_destroy(&globalLock), "pthread_mutex_destroy");
	
	for(int i = 0; i < N; i++)
		free(matrix[i]);
	free(matrix);
	free(tids);
	free(args);
	
	return 0;
}

void *threadFunc(void *arg)
{
	inData *data = (inData*)arg;
	
	double localMin = 0;
	int yes = 0;
	double *vec = matrix[data->idx];
	double x1 = vec[0];
	double y1 = vec[1];
	double x2, y2;
	int k = data->idx;
	double dim;
	
	for(int i = 0; i < N; i++)
	{
		if(i == k)
			continue;
		x2 = matrix[i][0];
		y2 = matrix[i][1];
		
		dim = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
		
		if(!yes)
		{
			localMin = dim;
			yes = 1;
		}
		else
		{
			if(dim < localMin)
			{
				localMin = dim;
			}
		}
	}
	
	
	check_pthread(pthread_mutex_lock(&globalLock), "pthread_mutex_lock");
	
	if(!init)
	{
		globalMin = localMin;
		init = 1;
	}
	else
	{
		if(localMin < globalMin)
			globalMin = localMin;
	}
	
	check_pthread(pthread_mutex_unlock(&globalLock), "pthread_mutex_unlock");
	
	return NULL;
}
