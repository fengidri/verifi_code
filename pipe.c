#define _GNU_SOURCE	       /* See feature_test_macros(7) */
#include <sched.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>	  /* For SYS_xxx definitions */

static void cpu_aff(int cpu)
{
	pid_t tid;
	int ret;
 	cpu_set_t mask;

    	CPU_ZERO(&mask);
    	CPU_SET(cpu, &mask); /* add CPU0 to cpu set */

	tid = syscall(SYS_gettid);

	ret = sched_setaffinity(tid, sizeof(cpu_set_t), &mask);
	if (ret) {
		printf("cpu aff set fail\n");
		exit(-1);
	}
}


static void do_child(int fd)
{
	char buf[1];
	while (1) {
		read(fd, buf, 1);
		write(fd, buf, 1);
	}
}

static void do_parent(int fd, int loop)
{
	char buf[1];
	struct timeval start, end;
	int _loop = loop;
	int64_t s;

	gettimeofday(&start, NULL);

	while (--_loop) {
		write(fd, buf, 1);
		read(fd, buf, 1);
	}

	gettimeofday(&end, NULL);

	s = (end.tv_sec - start.tv_sec) * 1000 * 1000 + end.tv_usec - start.tv_usec;

	printf("%lld.%0lld %lldus/%lld\n", s/loop, 1000 * (s % loop) / loop, s, loop);
}


int main(int argc, char *argv[])
{
	int pid;
       	int fds[2];
	int loop = 1000 * 100;
	int cpu1;
	int cpu2;

	if (pipe(fds)) {
		return -1;
	}

	if (argc < 3) {
		printf("./pipe cpu1 cpu2 [loop_n]\n");
		return -1;
	}

	cpu1 = atoi(argv[1]);
	cpu2 = atoi(argv[2]);
	if (argc >= 4) {
		loop = atoi(argv[3]);
	}


	pid = fork();

	if (pid == 0) {
		cpu_aff(cpu2);
		close(fds[0]);
		do_child(fds[1]);
	}
	else if (pid > 0) {
		cpu_aff(cpu1);
		close(fds[1]);
		do_parent(fds[0], loop);
	}
	else{
		return -1;
	}
	return 0;
}
