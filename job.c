#include <stdio.h>
#include <stdlib.h>
#include "job.h"
#include "queue.c"

#define CHANCE_OF_IO_REQUEST 10
#define CHANCE_OF_IO_COMPLETE 4

void print_job(job_t* job) {
    printf("%d:%d:%d:%d\n", job->pid, 
                            job->arrive_time, 
                            job->service_time, 
                            job->priority);
}

// helper function to create job based on the input
job_t* create_job(int pid, int arrival, int service, int proiority) {
    job_t* job = (job_t*)malloc(sizeof(job_t));
    job->pid = pid;
    job->arrive_time = arrival;
    job->service_time = service;
    job->priority = proiority;
    job->start_time = -1;    // When the process starts running
    job->end_time = -1;      // When the process finishes
    job->completed = 0;

            // Statistics tracking
    job->ready_time = 0;    // Time spent in ready queue
    job->io_time = 0;       // Time spent in I/O state
    return job;
}

// load the whole file of job into the target queue
void load_from_file(char* file, queue_t* queue) {
    FILE* load_file = fopen(file,"r");

    // check if load success
    if (load_file == NULL) {
        printf("Faile to open file\n");

    }

    // 4 variable to temperary store the reading numbers
    int n1, n2, n3, n4;

    // use a loop to iterate through all the lines in the file
    while (fscanf(load_file, "%d:%d:%d:%d", &n1, &n2, &n3, &n4) == 4) {
        // print to check if read properly
        //printf("Start loading from file: %d, %d, %d, %d\n", n1, n2, n3, n4);
        job_t* job_to_queue = create_job(n1,n2,n3,n4);
        enqueue(queue, job_to_queue);



        //printf("job added to queue:");
        //print_job((job_t*)peek_back(queue));

    }


}


// create job beased on the file readed
// this is only for testing and only read the first line
job_t* create_from_file(char* file_name) {
    job_t* return_job = NULL;

    int n1;
    int n2;
    int n3;
    int n4;

    FILE* file = fopen(file_name,"r");
    if (fscanf(file, "%d:%d:%d:%d", &n1, &n2, &n3, &n4) == 4) {
        printf("%d, %d, %d, %d\n", n1, n2, n3, n4);
        return_job = create_job(n1,n2,n3,n4);
    }

    return return_job;
}

// IO and rand stuff
int os_rand() {
    return rand();
}

// Seed the random number generator
void os_srand() {
    srand(1);  
}

int IO_request() {
    if (os_rand() % CHANCE_OF_IO_REQUEST == 0) {
        return 1;  // Request I/O
    } else {
        return 0;  // No I/O request
    }
}

int IO_complete() {
    if (os_rand() % CHANCE_OF_IO_COMPLETE == 0) {
        return 1;  // I/O complete
    } else {
        return 0;  // I/O not complete
    }
}