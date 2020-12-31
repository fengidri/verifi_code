#include <pthread.h>
#include <stdbool.h>


volatile int a;
volatile int b;
volatile int c;

void func2(void * _)
{
    int i = 0;
    sleep(1);
    a = 1;
    i = 0;
    while (true) {
        if (c) {
            break;
        }
        ++i;
    }
    printf("%d\n", i);
}


void func1(void *_)
{
    while (true) {
        if (a) {
            c = 1;
            break;
        }
    }
}


void main(void)
{
    pthread_t t1, t2;
    pthread_create(&t1, 0, func1, NULL);
    pthread_create(&t2, 0, func2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
