#ifndef JOB_H
#define JOB_H

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

void print_job(job_t* job);
job_t* create_job(int pid, int arrival, int service, int priority);
int load_from_file(char* file, queue_t* queue);


void os_srand();
int os_rand();
int IO_request();
int IO_complete();

#endif
