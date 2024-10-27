#ifndef JOB_H
#define JOB_H

typedef struct job {
    int pid;
    int arrive_time;
    int service_time;
    int priority;

} job_t;

#endif 