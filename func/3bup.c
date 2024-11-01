// set thread priority, set to run in single processor, explicit

#define _GNU_SOURCE
#include "func.h"
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/mman.h>

// each thread will execute one function and calculate response time

void* thread_f1(void* arg){
    int *args = (int *)arg;
    printf("[FUNCTION 1] Input parameters: %d, %d\n", args[0], args[1]);

    // call f1 and calculate response time
    struct timespec init, end;
    long long int f1_et = 0;

    clock_gettime(CLOCK_MONOTONIC, &init);
    f1(args[0], args[1]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if((end.tv_nsec - init.tv_nsec) < 0){
            f1_et = (end.tv_sec - init.tv_sec - 1) + (1000000000 + end.tv_nsec - init.tv_nsec);
    } else{
            f1_et = (end.tv_sec - init.tv_sec) + (end.tv_nsec - init.tv_nsec);
    }

    long long int *thr_f1_out = (long long int*)malloc(sizeof(long long int));
    *thr_f1_out = f1_et;

    free(args);
    pthread_exit((void*)thr_f1_out);
}

void* thread_f2(void* arg){
    int *args = (int *)arg;
    printf("[FUNCTION 2] Input parameters: %d, %d\n", args[0], args[1]);

    // call f2 and calculate response time
    struct timespec init, end;
    long long int f2_et = 0;

    clock_gettime(CLOCK_MONOTONIC, &init);
    f2(args[0], args[1]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if((end.tv_nsec - init.tv_nsec) < 0){
            f2_et = (end.tv_sec - init.tv_sec - 1) + (1000000000 + end.tv_nsec - init.tv_nsec);
    } else{
            f2_et = (end.tv_sec - init.tv_sec) + (end.tv_nsec - init.tv_nsec);
    }

    long long int *thr_f2_out = (long long int*)malloc(sizeof(long long int));
    *thr_f2_out = f2_et;

    free(args);
    pthread_exit((void*)thr_f2_out);
}

void* thread_f3(void* arg){
    int *args = (int *)arg;
    printf("[FUNCTION 3] Input parameters: %d, %d\n", args[0], args[1]);

    // call f3 and calculate response time
    struct timespec init, end;
    long long int f3_et = 0;

    clock_gettime(CLOCK_MONOTONIC, &init);
    f3(args[0], args[1]);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if((end.tv_nsec - init.tv_nsec) < 0){
            f3_et = (end.tv_sec - init.tv_sec - 1) + (1000000000 + end.tv_nsec - init.tv_nsec);
    } else{
            f3_et = (end.tv_sec - init.tv_sec) + (end.tv_nsec - init.tv_nsec);
    }

    long long int *thr_f3_out = (long long int*)malloc(sizeof(long long int));
    *thr_f3_out = f3_et;

    free(args);
    pthread_exit((void*)thr_f3_out);
}

int main(int argc, char* argv[]){
    // make sure process is not eligible for swapping
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // set highest priority to process to ensure it runs in real time
    int pid = getpid();
    setpriority(PRIO_PROCESS, pid, 19);

    // create single core mask and apply it to the process affinity, ensuring it runs on only one core (CPU)
    cpu_set_t single_core;
    CPU_ZERO(&single_core);
    CPU_SET(0, &single_core);
    sched_setaffinity(pid, sizeof(cpu_set_t), &single_core);

    // initialize thread related variables
    pthread_t thr_f1, thr_f2, thr_f3;
    pthread_attr_t attr_f1, attr_f2, attr_f3;
    struct sched_param param_f1, param_f2, param_f3;

    // initialize pthread attributes
    pthread_attr_init(&attr_f1);
    pthread_attr_init(&attr_f2);
    pthread_attr_init(&attr_f3);

    // set scheduling mode to FIFO (real-time policy)
    pthread_attr_setschedpolicy(&attr_f1, SCHED_FIFO);
    pthread_attr_setschedpolicy(&attr_f2, SCHED_FIFO);
    pthread_attr_setschedpolicy(&attr_f3, SCHED_FIFO);

    // set priorities
    param_f1.sched_priority = 1;
    param_f2.sched_priority = 2;
    param_f3.sched_priority = 3;

    pthread_attr_setschedparam(&attr_f1, &param_f1);
    pthread_attr_setinheritsched(&attr_f1, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedparam(&attr_f2, &param_f2);
    pthread_attr_setinheritsched(&attr_f2, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedparam(&attr_f3, &param_f3);
    pthread_attr_setinheritsched(&attr_f3, PTHREAD_EXPLICIT_SCHED);

    int *thr_f1_in = (int*)malloc(sizeof(int)*2);
    int *thr_f2_in = (int*)malloc(sizeof(int)*2);
    int *thr_f3_in = (int*)malloc(sizeof(int)*2);

    thr_f1_in[0] = 1;
    thr_f1_in[1] = 1;

    thr_f2_in[0] = 2;
    thr_f2_in[1] = 2;

    thr_f3_in[0] = 3;
    thr_f3_in[1] = 3;

    // create pthreads
    pthread_create(&thr_f1, NULL, &thread_f1, (void*)thr_f1_in);
    pthread_create(&thr_f2, NULL, &thread_f2, (void*)thr_f2_in);
    pthread_create(&thr_f3, NULL, &thread_f3, (void*)thr_f3_in);

    // time in ns
    long long int *thr_f1_out;
    long long int *thr_f2_out;
    long long int *thr_f3_out;

    pthread_join(thr_f1, (void **)&thr_f1_out);
    pthread_join(thr_f2, (void **)&thr_f2_out);
    pthread_join(thr_f3, (void **)&thr_f3_out);

    printf("[FUNCTION 1] Execution time: %d (ms)\n", (int)(*thr_f1_out/1e6));
    printf("[FUNCTION 2] Execution time: %d (ms)\n", (int)(*thr_f2_out/1e6));
    printf("[FUNCTION 3] Execution time: %d (ms)\n", (int)(*thr_f3_out/1e6));

    // receive response time as parameter in thr_out, print/do before free
    free(thr_f1_in);
    free(thr_f1_out);
    free(thr_f2_in);
    free(thr_f2_out);
    free(thr_f3_in);
    free(thr_f3_out);

    pthread_exit(NULL);

    munlockall();

    return 0;
}












