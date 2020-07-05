#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

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
	
#define NUMS (26)

typedef struct{
	char *file;
	int idx;
}inData;

pthread_mutex_t globalLock = PTHREAD_MUTEX_INITIALIZER;
int letters[NUMS];
int max;
char letter;

void *threadFunc(void *arg);

int main(int argc, char** argv)
{
	check(argc == 2, "argc");
	
	pthread_t *tids = malloc(NUMS * sizeof(pthread_t));
	check(tids != NULL, "malloc");
	inData *args = malloc(NUMS * sizeof(inData));
	check(args != NULL, "malloc");
	
	for(int i = 0; i < NUMS; i++)
	{
		args[i].file = argv[1];
		args[i].idx = i;
		check_pthread(pthread_create(&tids[i], NULL, threadFunc, &args[i]), "pthread_create");
	}
	
	for(int i = 0; i < NUMS; i++)
	{
		check_pthread(pthread_join(tids[i], NULL), "pthread_join");
	}
	
	for(int i = 0; i < NUMS; i++)
	{
		printf("%d ", letters[i]);
	}
	printf("\n%c\n", letter);
	
	free(tids);
	free(args);
	
	return 0;
}

void *threadFunc(void *arg)
{
	inData *data = (inData*)arg;
	
	int count = 0;
	int id = data->idx;
	char *file = data->file;
	char savedLetter;
	
	FILE *f = fopen(file, "r");
	check(f != NULL, "fopen");
	
	char *line = NULL;
	size_t lineLen = 0;
	
	while(getline(&line, &lineLen, f) > 0)
	{
		int len = strlen(line);
		for(int i = 0; i < len; i++)
		{
			char ch = (unsigned char)line[i];
			if(!isalpha(ch))
				continue;
			int c = (int)(tolower(ch) - 'a');
			if(c == id)
			{
				count++;
				savedLetter = ch;
			}
		}
	}
	
	check(pthread_mutex_lock(&(globalLock)) != -1, "pthread_mutex_lock");
	
	letters[id] = count;
	if(count > max)
	{
		max = count;
		letter = savedLetter;
	}
	
	check(pthread_mutex_unlock(&(globalLock)) != -1, "pthread_mutex_unlock");
	
	fclose(f);
	return NULL;
}
