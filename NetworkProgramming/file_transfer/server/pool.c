#include "pool.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>

static pool *mypool = NULL;

static void* thread_routine(void *arguments) {
	pool_work *working;
	while (1) {
		pthread_mutex_lock(&mypool->lock);
		while (!mypool->head && !mypool->shutdown)
			pthread_cond_wait(&mypool->ready, &mypool->lock);

		if (mypool->shutdown) {
			pthread_mutex_unlock(&mypool->lock);
			pthread_exit(NULL);
		}

		working = mypool->head;
		mypool->head = mypool->head->next;
		pthread_mutex_unlock(&mypool->lock);
		working->routine(working->arguments);
		// free((void*)working->arguments);
		free(working);
	}
	return NULL;
}

int pool_create(int total) {
	mypool = calloc(1, sizeof(pool));
	if (!mypool) {
		fprintf(stderr, "allocate pool fialed!\n");
		exit(-1);
	}

	mypool->max_number = total;
	mypool->shutdown = 0;
	mypool->head = NULL;
	mypool->tail = NULL;

	if (pthread_mutex_init(&mypool->lock, NULL) != 0) {
		fprintf(stderr, "pthread mutex initialization failed\n");
		exit(-1);
	}

	if (pthread_cond_init(&mypool->ready, NULL) != 0) {
		fprintf(stderr, "pthread cond initialization failed\n");
		exit(-1);
	}

	mypool->thread_id = calloc(total, sizeof(pthread_t));
	if (!mypool->thread_id) {
		fprintf(stderr, "allocate thread fialed!\n");
		exit(-1);
	}

	int i;
	for (i = 0;i < total;i++) {
		if (pthread_create(&mypool->thread_id[i], NULL, thread_routine, NULL) != 0) {
			fprintf(stderr, "create thread failed,error no:%d %s\n", errno, strerror(errno));
			exit(-1);
		}
	}

	return 0;
}

int add_work(void*(*routine)(int*), int* arg) {
	pool_work *work;
	work = malloc(sizeof(pool_work));
	if (!work) {
		fprintf(stderr, "malloc work thread memory failed\n");
		return -1;
	}

	work->routine = routine;
	work->arguments = arg;
	work->next = NULL;

	pthread_mutex_lock(&mypool->lock);

	if (!mypool->head) {
		mypool->head = work;
		mypool->tail = work;	
	} else {
		mypool->tail->next = work;
		mypool->tail = work;
	}

	pthread_cond_signal(&mypool->ready);
	pthread_mutex_unlock(&mypool->lock);
	return 0;
}	

void destroy() {
	if (mypool->shutdown)
		return;

	mypool->shutdown = 1;
	pthread_mutex_lock(&mypool->lock);
	pthread_cond_broadcast(&mypool->ready);
	pthread_mutex_unlock(&mypool->lock);

	int i;
	for (i = 0;i < mypool->max_number;i++)
		pthread_join(mypool->thread_id[i], NULL);

	free(mypool->thread_id);

	pool_work* working;
	while (mypool->head) {
		working = mypool->head;
		mypool->head = mypool->head->next;
		// free((void*)working->arguments);
		free(working);
	}

	pthread_mutex_destroy(&mypool->lock);
	pthread_cond_destroy(&mypool->ready);

	free(mypool);
}