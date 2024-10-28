#ifndef JOB_H
#define JOB_H

#include <stdio.h>
#include "queue.h"

typedef struct job {
    int pid;
    int arrive_time;
    int service_time;
    int priority;
    int start_time;
    int end_time;
    int completed;
    int ready_time;
    int io_time;
    int cpu_time;
} job_t;

job_t* create_job(int pid, int arrival, int service, int priority);
void load_from_file(char* file, queue_t* queue);
void print_job(job_t* job);


void os_srand();
int IO_request();
int IO_complete();

#endif
