#ifndef JOB_H
#define JOB_H


#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Constants for IO request/completion probability
#define CHANCE_OF_IO_REQUEST 10
#define CHANCE_OF_IO_COMPLETE 4


typedef struct job {
    int pid;
    int arrive_time;
    int service_time;
    int priority;
    
    int start_time;    // When the process starts running
    int end_time;      // When the process finishes
    int completed;

    // Statistics tracking
    int ready_time;    // Time spent in ready queue
    int io_time;       // Time spent in I/O state
    int cpu_time;      // Time running on CPU

} job_t;

void print_job(job_t* job);
job_t* create_job(int pid, int arrival, int service, int priority);
void load_from_file(char* file, queue_t* queue);
job_t* create_from_file(char* file_name);
int os_rand();
void os_srand();
int IO_request();
int IO_complete();


#endif 