#ifndef JOB_H
#define JOB_H

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

#endif 