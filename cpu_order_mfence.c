#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>

#include <stdlib.h>

#include <semaphore.h>
/*
 *
 *
 * */
sem_t sem1, sem2, seme;

int X, Y;
int r1, r2;

void *fun1(void *x)
{
	while (1) {
		sem_wait(&sem1);
		X = 1;
		asm volatile("mfence" ::: "memory"); // Prevent compiler reordering
		r1 = Y;
		sem_post(&seme);
	}
	return NULL;
}

void *fun2(void *x)
{
	while (1) {
		sem_wait(&sem2);
		Y = 1;
		asm volatile("mfence" ::: "memory"); // Prevent cpu reordering
		r2 = X;
		sem_post(&seme);
	}
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t t1, t2;


	pthread_create(&t1, NULL, fun1, NULL);
	pthread_create(&t2, NULL, fun2, NULL);

	int detected = 0;
	int iter;
	for (iter = 1; ; ++iter) {
		X = 0;
		Y = 0;
		sem_post(&sem1);
		sem_post(&sem2);
		sem_wait(&seme);
		sem_wait(&seme);
		if (r1 == 0 && r2 == 0) {
			++detected;
			printf("detected: %d/%d\n", detected, iter);
			iter = 0;
		}
	}

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return 0;
}
