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
	
typedef struct yes{
	int idx;
}inData;

typedef struct no{
	int idx;
	float max;
}outData;

float **matrix;
int m, n, k;
int globalId;
float globalMax = 0;
int globalInit = 0;

void *threadFunc(void *arg);

int main(int argc, char **argv)
{
	check(argc == 1, "argc");
	
	scanf("%d %d %d", &m, &n, &k);
	
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
	
	pthread_t *tids = malloc(k * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	inData *args = malloc(k * sizeof(inData));
	check(args != NULL, "malloc");
	
	for(int i = 0; i < k; i++)
	{
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < k; i++)
	{
		outData *retVal = NULL;
		check_pthread(pthread_join(tids[i], (void**)&retVal), "pthread_join");
		
		if(!globalInit)
		{
			globalMax = retVal->max;
			globalId = retVal->idx;
			globalInit = 1;
		}
		else
		{
			if(retVal->max > globalMax)
			{
				globalMax = retVal->max;
				globalId = retVal->idx;
			}
		}
	}
	printf("%d %f\n", globalId, globalMax);
	
	for(int i = 0; i < m; i++)
		free(matrix[i]);
	free(matrix);
	free(tids);
	free(args);
	
	return 0;
}

void *threadFunc(void *arg)
{
	inData *data = (inData*)arg;
	outData *out = malloc(sizeof(outData));
	check(out != NULL, "malloc");
	
	int id = data->idx;
	float norm;
	float sum;
	float localMax = 0;
	int init = 0;
	
	for(int i = k*id; i < k*id + k; i++)
	{
		sum = 0;
		for(int j = 0; j < n; j++)
		{
			sum += matrix[i][j] * matrix[i][j];
		}
		norm = sqrtf(sum);
		
		if(!init)
		{
			localMax = norm;
			init = 1;
		}
		else
		{
			if(norm > localMax)
				localMax = norm;
		}
	}
	
	out->idx = id;
	out->max = localMax;
	
	return out;
}	
