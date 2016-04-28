#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

void *printInfo(void *threadid)
{
    long tid;
    tid = (long)threadid;
    do
    {
        sleep(5);
        printf("i'm printing what should be account information, from thread %ld\n",tid);
    }
    while(1);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    int rc;
    long t;
    printf("in main creating thread\n");
    rc = pthread_create(&thread, NULL, printInfo, (void*)t);
    if(rc)
    {
        printf("error, return code is: %d\n",rc);
        exit(-1);
    }

    pthread_exit(NULL);
}