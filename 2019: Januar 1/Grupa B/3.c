#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <pthread.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define check_thread(err, msg) \
	do { \
		int _err = err; \
		if(_err > 0) { \
			errno = _err; \
			check(0, msg); \
		} \
	}while(0)
	
typedef struct{
	int idx;
}inData;

typedef struct{
	int idx;
	float max;
}outData;

float **matrix;
float *vector;
float globalMax = 0;
int m, n, k;
int numThreadMax = 0;
int initialized = 0;
//pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;

void *threadFunc(void *arg);

int main(int argc, char **argv)
{
	check(argc == 1, "argc");
	
	scanf("%d%d%d", &m, &n, &k);
	
	matrix = malloc(m * sizeof(float*));
	check(matrix != NULL, "malloc");
	
	for(int i = 0; i < m; i++)
	{
		matrix[i] = malloc(n * sizeof(float));
		check(matrix[i] != NULL, "malloc");
	}
	
	for(int i = 0; i < m; i++)
		for(int j = 0; j < n; j++)
			scanf("%f", &matrix[i][j]);
	
	vector = malloc(n * sizeof(float));
	check(vector != NULL, "malloc");
	
	for(int i = 0; i < n; i++)
		scanf("%f", &vector[i]);
	
	pthread_t *tids = malloc(k * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	inData* data = malloc(k * sizeof(inData));
	check(data != NULL, "malloc");
	
	for(int i = 0; i < k; i++)
	{
		data[i].idx = i;
		check_thread(pthread_create(&tids[i], NULL, threadFunc, &data[i]), "pthread_create");
	}
	
	for(int i = 0; i < k; i++)
	{
		outData *retVal = NULL;
		check_thread(pthread_join(tids[i], (void**)&retVal), "pthread_join");
		
		if(!initialized)
		{
			globalMax = retVal->max;
			numThreadMax = retVal->idx;
			initialized = 1;
		}
		else
		{
			if(retVal->max > globalMax)
			{
				globalMax = retVal->max;
				numThreadMax = retVal->idx;
			}
		}
	}
	
	printf("%d %f", numThreadMax, globalMax);
	
	for(int i = 0; i < m; i++)
		free(matrix[i]);
	free(matrix);
	free(tids);
	free(data);
	
	return 0;
}

void *threadFunc(void *arg)
{
	inData *data = (inData*)arg;
	
	outData *out = malloc(sizeof(outData));
	check(out != NULL, "malloc");
	
	float localMax = 0;
	int init = 1;
	int thrNum = data->idx;
	
	for(int i = thrNum*k; i < thrNum*k + k; i++)
	{
		float curr = 0;
		for(int j = 0; j < n; j++)
			curr += matrix[i][j] * vector[j];
		
		if(init)
		{
			localMax = curr;
			init = 0;
		}
		else
		{
			if(curr > localMax)
				localMax = curr;
		}
	}
	
	out->idx = data->idx + 1;
	out->max = localMax;
	
	return out;
	
	/*check_thread(pthread_mutex_lock(&globalLock), "pthread_mutex_lock");
	
	if(!initialized)
	{
		globalMax = localMax;
		numThreadMax = data->idx;
		initialized = 1;
	}
	else
	{
		if(localMax > globalMax)
		{
			globalMax = localMax;
			numThreadMax = data->idx;
		}
	}
	
	check_thread(pthread_mutex_unlock(&globalLock), "pthread_mutex_unlock");
	
	return NULL;*/
}
