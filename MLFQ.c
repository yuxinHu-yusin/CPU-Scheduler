#include <stdlib.h>
#include <stdio.h>
#include "job.h"
#include "queue.h"

#define NUM_QUEUES 3
#define TIME_QUANTUM_LEVEL1 2
#define TIME_QUANTUM_LEVEL2 4
#define BOOST_INTERVAL 20

typedef struct {
    queue_t *queues[NUM_QUEUES];
    int time_quantum[NUM_QUEUES];
    int current_boost_time;
} MLFQScheduler;

MLFQScheduler *init_mlfq_scheduler() {
    MLFQScheduler *scheduler = malloc(sizeof(MLFQScheduler));
    scheduler->queues[0] = create_queue();
    scheduler->queues[1] = create_queue();
    scheduler->queues[2] = create_queue();
    scheduler->time_quantum[0] = TIME_QUANTUM_LEVEL1;
    scheduler->time_quantum[1] = TIME_QUANTUM_LEVEL2;
    scheduler->time_quantum[2] = TIME_QUANTUM_LEVEL2 * 2;
    scheduler->current_boost_time = 0;
    return scheduler;
}

void boost_all_jobs(MLFQScheduler *scheduler) {
    for (int i = 1; i < NUM_QUEUES; i++) {
        while (!is_empty(scheduler->queues[i])) {
            job_t *job = dequeue(scheduler->queues[i]);
            enqueue(scheduler->queues[0], job);
        }
    }
    scheduler->current_boost_time = 0;
}

void enqueue_job(MLFQScheduler *scheduler, job_t *job, int queue_level) {
    if (queue_level >= NUM_QUEUES) queue_level = NUM_QUEUES - 1;
    enqueue(scheduler->queues[queue_level], job);
}

job_t *get_next_job(MLFQScheduler *scheduler) {
    for (int i = 0; i < NUM_QUEUES; i++) {
        if (!is_empty(scheduler->queues[i])) {
            return dequeue(scheduler->queues[i]);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int clock = 0;
    os_srand();

    MLFQScheduler *scheduler = init_mlfq_scheduler();
    queue_t *job_queue = create_queue();
    queue_t *wait_queue = create_queue();
    queue_t *finished_queue = create_queue();

    if (argc == 2) {
        load_from_file(argv[1], job_queue);
    } else {
        printf("Please provide a job file as an argument.\n");
        return 1;
    }

    while (!is_empty(job_queue) || !is_empty(wait_queue) || !is_empty(scheduler->queues[0]) || !is_empty(scheduler->queues[1]) || !is_empty(scheduler->queues[2])) {
        if (scheduler->current_boost_time >= BOOST_INTERVAL) {
            boost_all_jobs(scheduler);
        }

        while (!is_empty(job_queue) && ((job_t *)job_queue->front->data)->arrive_time <= clock) {
            job_t *new_job = dequeue(job_queue);
            enqueue_job(scheduler, new_job, 0);
        }

        int wait_count = wait_queue->count;
        for (int i = 0; i < wait_count; i++) {
            job_t *io_job = dequeue(wait_queue);
            io_job->io_time++;
            if (IO_complete()) {
                enqueue_job(scheduler, io_job, 0);
            } else {
                enqueue(wait_queue, io_job);
            }
        }

        job_t *current_job = get_next_job(scheduler);
        if (current_job) {
            int queue_level = current_job->priority;
            int time_slice = scheduler->time_quantum[queue_level];
            int runtime = (current_job->service_time < time_slice) ? current_job->service_time : time_slice;

            clock += runtime;
            current_job->service_time -= runtime;

            if (current_job->service_time == 0) {
                current_job->end_time = clock;
                current_job->completed = 1;
                enqueue(finished_queue, current_job);
            } else if (IO_request()) {
                enqueue(wait_queue, current_job);
            } else {
                int next_queue = (queue_level + 1 < NUM_QUEUES) ? queue_level + 1 : NUM_QUEUES - 1;
                enqueue_job(scheduler, current_job, next_queue);
            }
        } else {
            clock++;
        }

        scheduler->current_boost_time++;
    }

    printf("        |  Total time       |  Total time       |  Total time   |\n");
    printf("Job#    |  in ready to run  |  in sleeping on   |  in system    |\n");
    printf("        |  state            |  I/O state        |               |\n");
    printf("========+===================+===================+===============+\n");

    node_t *temp = finished_queue->front;
    int total_time_comp = 0, total_time_ready = 0, total_time_io = 0;

    while (temp != NULL) {
        job_t *job = (job_t *)temp->data;
        int time_in_system = job->end_time - job->start_time;
        total_time_comp += time_in_system;
        total_time_ready += job->ready_time;
        total_time_io += job->io_time;

        printf("pid%4d |  %-17d|  %-17d|  %-13d|\n", job->pid, job->ready_time, job->io_time, time_in_system);
        temp = temp->next;
    }

    printf("================================================================+\n");
    printf("Total simulation run time: %d\n", clock);
    printf("Average job completion time: %.3f\n", total_time_comp / (float)finished_queue->count);
    printf("Average time in ready queue: %.3f\n", total_time_ready / (float)finished_queue->count);
    printf("Average time sleeping on I/O: %.3f\n", total_time_io / (float)finished_queue->count);

    return 0;
}
