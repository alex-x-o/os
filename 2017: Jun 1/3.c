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

int n, m, m1, k;
int **matricaA;
int **matricaB;
int **matricaC;
pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;
int globalMax;
int init = 0;

typedef struct {
	int idx;
} data_t;

void *threadFunc(void *arg)
{
	data_t* data = (data_t *)arg;
	
	int p = data->idx / k;
	int d = data->idx % k;
	int res = 0;
	
	for(int i = 0; i < m; i++)
	{
		res += matricaA[p][i] * matricaB[i][d];
	}
	matricaC[p][d] = res;
	int localMax = matricaC[p][d];
	
	check_pthread(pthread_mutex_lock(&globalLock), "lock");
	
	if(init)
	{
		if(localMax > globalMax)
			globalMax = localMax;
	}
	else
	{
		globalMax = localMax;
		init = 1;
	}
	
	check_pthread(pthread_mutex_unlock(&globalLock), "unlock");

	return NULL;
}

int main(int argc, char **argv)
{
	check(argc == 1, "Usage: ./3");
		
	scanf("%d %d", &n, &m);
	matricaA = malloc(n * sizeof(int*));
	check(matricaA != NULL, "malloc");
	
	for(int i = 0; i < n; i++)
	{
		matricaA[i] = malloc(m * sizeof(int));
		check(matricaA[i] != NULL, "malloc");
	}
	
	for(int i = 0; i < n; i++)
		for(int j = 0; j < m; j++)
			scanf("%d", &matricaA[i][j]);
	
	scanf("%d %d", &m1, &k);
	check(m == m1, "not equal");
	
	matricaB = malloc(m * sizeof(int*));
	check(matricaB != NULL, "malloc");
	
	for(int i = 0; i < m; i++)
	{
		matricaB[i] = malloc(k * sizeof(int));
		check(matricaB[i] != NULL, "malloc");
	}
	
	for(int i = 0; i < m; i++)
		for(int j = 0; j < k; j++)
			scanf("%d", &matricaB[i][j]);
	
	matricaC = malloc(n * sizeof(int*));
	check(matricaC != NULL, "malloc");
	
	for(int i = 0; i < n; i++)
	{
		matricaC[i] = malloc(k * sizeof(int));
		check(matricaC[i] != NULL, "malloc");
	}
	
	pthread_t* tids = malloc(n * k * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	data_t *args = malloc(n * k * sizeof(data_t));
	check(args != NULL, "malloc");
	
	for(int i = 0; i < n * k; i++)
	{
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < n * k; i++)
	{
		check_pthread(pthread_join(tids[i], NULL), "pthread_join");
	}
	
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < k; j++)
			printf("%d ", matricaC[i][j]);
		printf("\n");
	}
		
	printf("%d", globalMax);
	
	for(int i = 0; i < n; i++)
		free(matricaA[i]);
	free(matricaA);
	
	for(int i = 0; i < m; i++)
		free(matricaB[i]);
	free(matricaB);
	
	for(int i = 0; i < n; i++)
		free(matricaC[i]);
	free(matricaC);
	
	free(tids);
	free(args);
	
	return 0;
}
