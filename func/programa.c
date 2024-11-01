//  ------------------------------
//  | Real Time Systems, DEEC/UC |
//  | Miguel Hirche Almeida      |
//  | Andre Gamito Vilhena       |
//  | 01/10/2024                 |
//  ------------------------------

// RUN AS SUPERUSER

#define _GNU_SOURCE // macros & sched_setaffinity()
#include "func.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/mman.h>

int main(int argc, char* argv[]){
    // make sure process is not eligible for swapping
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // set highest priority to process to ensure it runs in real time
    int pid = getpid();
    setpriority(PRIO_PROCESS, pid, -20);

    // create single core mask and apply it to the process affinity, ensuring it runs on only one core (CPU)
    cpu_set_t single_core;
    CPU_ZERO(&single_core);
    CPU_SET(0, &single_core);
    sched_setaffinity(pid, sizeof(cpu_set_t), &single_core);

    // initialize time variables
    struct timespec init_f1, end_f1, init_f2, end_f2, init_f3, end_f3;
    long long int tmp_f1_et, tmp_f2_et, tmp_f3_et;
    long long int f1_et = 0;
    long long int f2_et = 0;
    long long int f3_et = 0;

    for(int i = 0; i < 1000; i++){
        // call functions and get initial/end time
        clock_gettime(CLOCK_MONOTONIC, &init_f1);
        f1(1, 1);
        clock_gettime(CLOCK_MONOTONIC, &end_f1);

        clock_gettime(CLOCK_MONOTONIC, &init_f2);
        f2(2, 2);
        clock_gettime(CLOCK_MONOTONIC, &end_f2);

        clock_gettime(CLOCK_MONOTONIC, &init_f3);
        f3(3, 3);
        clock_gettime(CLOCK_MONOTONIC, &end_f3);

        // time elapsed in seconds, plus time elapsed in nanoseconds, since time in ns is only counted in between seconds. also ensure that the absolute difference in nanoseconds is calculated, in case the initial time is bigger than the end time due to end time being a second (or more) ahead.
        if((end_f1.tv_nsec - init_f1.tv_nsec) < 0){
                tmp_f1_et = (end_f1.tv_sec - init_f1.tv_sec - 1) + (1000000000 + end_f1.tv_nsec - init_f1.tv_nsec);
        } else{
                tmp_f1_et = (end_f1.tv_sec - init_f1.tv_sec) + (end_f1.tv_nsec - init_f1.tv_nsec);
        }

        if((end_f2.tv_nsec - init_f2.tv_nsec) < 0){
                tmp_f2_et = (end_f2.tv_sec - init_f2.tv_sec - 1) + (1000000000 + end_f2.tv_nsec - init_f2.tv_nsec);
        } else{
                tmp_f2_et = (end_f2.tv_sec - init_f2.tv_sec) + (end_f2.tv_nsec - init_f2.tv_nsec);
        }

        if((end_f3.tv_nsec - init_f3.tv_nsec) < 0){
                tmp_f3_et = (end_f3.tv_sec - init_f3.tv_sec - 1) + (1000000000 + end_f3.tv_nsec - init_f3.tv_nsec);
        } else{
                tmp_f3_et = (end_f3.tv_sec - init_f3.tv_sec) + (end_f3.tv_nsec - init_f3.tv_nsec);
        }

        if(tmp_f1_et > f1_et){
            f1_et = tmp_f1_et;
        }

        if(tmp_f2_et > f2_et){
            f2_et = tmp_f2_et;
        }

        if(tmp_f3_et > f3_et){
            f3_et = tmp_f3_et;
        }

        if(!((i+1)%10)){
            printf("Execution %d\n", i+1);
        }
    }

    printf("\n");
    printf("[FUNCTION 1] Maximum execution time (1000 executions): %d (ms)\n", (int)(f1_et/1e6));
    printf("[FUNCTION 2] Maximum execution time (1000 executions): %d (ms)\n", (int)(f2_et/1e6));
    printf("[FUNCTION 3] Maximum execution time (1000 executions): %d (ms)\n", (int)(f3_et/1e6));

    // eligible for swapping
    munlockall();

    return 0;
}
