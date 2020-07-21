#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>

#include <stdlib.h>

/*
 *
 * for cache line false share
 * gcc -O0 cache_line_false_share.c -l pthread
 * ./a.out 1
 * ./a.out 100
 *
 *
 * */

int a[20000];

int windex;

void *fun1(void *x)
{
	long long i;
	int s;

	struct timeval start, end;

	gettimeofday(&start, NULL);


	for (i = 0; i < 1000000000; ++i) {
		s = s + a[0];
	}
	gettimeofday(&end, NULL);
	printf("1 %lldus\n", (end.tv_sec - start.tv_sec) * 1000 * 1000 + end.tv_usec - end.tv_usec);
	return NULL;
}

void *fun2(void *x)
{
	long long i;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	for (i = 0; i < 1000000000; ++i) {
		a[windex] = 100 + i;
	}
	gettimeofday(&end, NULL);
	printf("2 %lldus\n", (end.tv_sec - start.tv_sec) * 1000 * 1000 + end.tv_usec - end.tv_usec);
	return NULL;
}

int main(int argc, char **argv)
{
	printf("%p %d\n", a, (long long)a % 64);
	pthread_t t1, t2;

	windex = atoi(argv[1]);

	pthread_create(&t1, NULL, fun1, NULL);
	pthread_create(&t2, NULL, fun2, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
