#ifndef _POOL_H_
#define _POOL_H_

#include <pthread.h>

typedef struct pool_work_s {
	void* (*routine)(int*);
	// void *arguments;
	int* arguments;
	struct pool_work_s *next;
}pool_work;

typedef struct pool_s {
	int shutdown;
	int max_number;
	pthread_t *thread_id;
	pool_work *head;
	pool_work *tail;
	pthread_mutex_t lock;
	pthread_cond_t ready;
}pool;

int pool_create(int total);
int add_work(void*(*routine)(int*), int* arg);
void destroy();

#endif