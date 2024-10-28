#include <stdio.h>
#include <stdlib.h>
#include "job.h"
#include "queue.h"

#define CHANCE_OF_IO_REQUEST 10
#define CHANCE_OF_IO_COMPLETE 4

void print_job(job_t* job) {
    printf("%d:%d:%d:%d\n", job->pid, job->arrive_time, job->service_time, job->priority);
}

job_t* create_job(int pid, int arrival, int service, int priority) {
    job_t* job = (job_t*)malloc(sizeof(job_t));
    job->pid = pid;
    job->arrive_time = arrival;
    job->service_time = service;
    job->priority = priority;
    job->start_time = -1;
    job->end_time = -1;
    job->completed = 0;
    job->ready_time = 0;
    job->io_time = 0;
    job->cpu_time = 0;
    return job;
}

int load_from_file(char* file, queue_t* queue) {  // 保持和 job.h 一致的 int 返回类型
    FILE* load_file = fopen(file, "r");
    if (load_file == NULL) {
        printf("Failed to open file: %s\n", file);
        return -1;  // 文件打开失败
    }

    int n1, n2, n3, n4;
    while (fscanf(load_file, "%d:%d:%d:%d", &n1, &n2, &n3, &n4) == 4) {
        job_t* job_to_queue = create_job(n1, n2, n3, n4);
        enqueue(queue, job_to_queue);
    }
    fclose(load_file);
    return 0;  // 文件成功加载
}

int os_rand() { return rand(); }
void os_srand() { srand(1); }

int IO_request() {
    return (os_rand() % CHANCE_OF_IO_REQUEST == 0) ? 1 : 0;
}

int IO_complete() {
    return (os_rand() % CHANCE_OF_IO_COMPLETE == 0) ? 1 : 0;
}
