// SRTF.c
#include <stdio.h>
#include <limits.h>
#include "job.h"
#include "queue.h"

#define test_mode 1

// Helper function to check if jobs have arrived
int check_arrive(queue_t* job_queue, queue_t* ready_queue, int clock);

// Helper function to dequeue the job with the shortest remaining time
job_t* dequeue_short(queue_t* job_queue);

int main(int argc, char* argv[]) {
    int clock = 0;
    os_srand();

    // Initialize queues
    queue_t* job_queue = create_queue();        // Stores all jobs loaded from the file
    queue_t* ready_queue = create_queue();      // Jobs ready to run
    queue_t* wait_queue = create_queue();       // Jobs waiting for I/O
    queue_t* finished_queue = create_queue();   // Completed jobs

    // Load jobs from file
    if (test_mode) {
        if (load_from_file("job1.txt", job_queue) == -1) {
            fprintf(stderr, "Error: Failed to open file job1.txt\n");
            return 1;
        }
    } else {
        if (argc != 2) {
            printf("Please provide one job file as an argument.\n");
            return 1;
        }
        if (load_from_file(argv[1], job_queue) == -1) {
            fprintf(stderr, "Error: Failed to open file %s\n", argv[1]);
            return 1;
        }
    }

    clock++;

    // Main loop to simulate CPU scheduling
    job_t* current_job = NULL;

    while (!is_empty(job_queue) || !is_empty(wait_queue) ||
           !is_empty(ready_queue) || current_job != NULL) {

        if (test_mode) {
            printf("Time: %d\n", clock);
            printf("job: %d  ready: %d  wait: %d\n",
                   job_queue->count, ready_queue->count, wait_queue->count);
        }

        // Check for job arrivals
        if (check_arrive(job_queue, ready_queue, clock)) {
            if (current_job != NULL) {
                enqueue(ready_queue, current_job);
            }
            current_job = dequeue_short(ready_queue);
        }

        // Process jobs in the wait queue
        int original_count = wait_queue->count;
        for (int i = 0; i < original_count; i++) {
            job_t* temp = dequeue(wait_queue); // Dequeue a job from wait queue
            temp->io_time++; // Increment IO waiting time
            if (IO_complete()) {
                if (test_mode) {
                    printf("IO Completed: %d\n", temp->pid);
                }
                enqueue(ready_queue, temp);
            } else {
                enqueue(wait_queue, temp); // Enqueue back if still waiting
            }
        }

        // Check if current job is NULL
        if (current_job == NULL) {
            // Get the next job from the ready queue
            current_job = dequeue_short(ready_queue);

            // No job is ready, advance the clock
            if (current_job == NULL) {
                clock++;
                continue;
            }
        }

        // Check if current job is completed
        if (current_job->service_time == 0) {
            current_job->end_time = clock;  // Record end time
            enqueue(finished_queue, current_job);  // Move to finished queue

            if (test_mode) {
                printf("A job completed: ");
                print_job(current_job);
                printf("\n");
            }

            current_job = dequeue_short(ready_queue); // Get next job
            continue;
        }

        // Set start time when the job first starts
        if (current_job->start_time == -1) {
            current_job->start_time = clock;
        }

        // Check if current job requests I/O
        int state = IO_request();

        if (test_mode) {
            printf("Current Job: %d  Time Left: %d\n", current_job->pid, current_job->service_time);
            printf("IO Request: %d\n", state);
            printf("\n");
        }

        if (state == 1) {
            enqueue(wait_queue, current_job);

            if (!is_empty(ready_queue)) {
                // Switch to the next job if available
                current_job = dequeue_short(ready_queue);
                continue;
            } else {
                // No other job ready, set current_job to NULL
                current_job = NULL;
                clock++;
                continue;
            }
        }

        // Add 1 to ready_time for all jobs in the ready queue
        node_t* temp_node = ready_queue->front;
        while (temp_node != NULL) {
            job_t* to_add = (job_t*)temp_node->data;
            to_add->ready_time++;
            temp_node = temp_node->next;
        }

        // Increment clock time
        clock++;

        // Decrease service_time and increment cpu_time if current_job is not NULL
        if (current_job != NULL) {
            current_job->service_time--;
            current_job->cpu_time++;
        }
    }

    // Print the results
    printf("        |  Total time       |  Total time       |  Total time   |\n");
    printf("Job#    |  in ready to run  |  in sleeping on   |  in system    |\n");
    printf("        |  state            |  I/O state        |               |\n");
    printf("========+===================+===================+===============+\n");

    // Iterate through finished queue to print out results
    node_t* temp = finished_queue->front;
    int short_time = INT_MAX; // Record the shortest job completion time
    int long_time = -1;       // Record the longest job completion time
    float total_time_comp = 0;
    float total_time_ready = 0;
    float total_time_io = 0;

    while (temp != NULL) {
        job_t* to_print = (job_t*)temp->data;

        // Update statistics
        int time_in_sys = to_print->end_time - to_print->start_time;
        total_time_comp += time_in_sys;
        total_time_ready += to_print->ready_time;
        total_time_io += to_print->io_time;
        if (time_in_sys < short_time) {
            short_time = time_in_sys;
        }
        if (time_in_sys > long_time) {
            long_time = time_in_sys;
        }

        // Print job statistics
        printf("pid%4d |  %-17d|  %-17d|  %-13d|\n",
               to_print->pid, to_print->ready_time, to_print->io_time, time_in_sys);

        temp = temp->next;
    }

    // Print the total statistics
    printf("================================================================+\n");
    printf("Total simulation run time: %d\n", clock);
    printf("Total number of jobs: %d\n", finished_queue->count);
    printf("Shortest job completion time: %d\n", short_time);
    printf("Longest job completion time: %d\n", long_time);
    printf("Average job completion time: %.3f\n", total_time_comp / finished_queue->count);
    printf("Average time in ready queue: %.3f\n", total_time_ready / finished_queue->count);
    printf("Average time sleeping on I/O: %.3f\n", total_time_io / finished_queue->count);
    printf("\n");

    // Clean up (optional, but good practice)
    free_queue(job_queue);
    free_queue(ready_queue);
    free_queue(wait_queue);
    free_queue(finished_queue);

    return 0;
}

// Helper function to check if jobs have arrived
int check_arrive(queue_t* job_queue, queue_t* ready_queue, int clock) {
    int is_arrive = 0;
    int original_count = job_queue->count;

    for (int i = 0; i < original_count; i++) {
        job_t* temp = dequeue(job_queue); // Dequeue a job from job_queue

        if (temp->arrive_time <= clock) {
            if (test_mode) {
                printf("Job arrived: %d\n", temp->pid);
            }
            is_arrive = 1;
            enqueue(ready_queue, temp);
        } else {
            enqueue(job_queue, temp); // Enqueue back if not yet arrived
        }
    }

    return is_arrive;
}

// Helper function to dequeue the job with the shortest remaining time
// If a tie occurs, the job with the smaller pid is dequeued
job_t* dequeue_short(queue_t* job_queue) {
    if (job_queue->count == 0) {
        return NULL;
    }

    job_t* short_job = NULL;
    int short_time = INT_MAX;
    int original_count = job_queue->count;

    for (int i = 0; i < original_count; i++) {
        job_t* temp = dequeue(job_queue); // Dequeue a job

        if (temp->service_time < short_time ||
            (temp->service_time == short_time && temp->pid < (short_job ? short_job->pid : INT_MAX))) {
            if (short_job != NULL) {
                enqueue(job_queue, short_job); // Enqueue the previous shortest job back
            }
            short_job = temp;
            short_time = short_job->service_time;
        } else {
            enqueue(job_queue, temp); // Enqueue back if not the shortest
        }
    }

    return short_job;
}
