// coordinator.c
#include <stdio.h>
#include <limits.h>
#include "job.h"
#include "queue.h"


#define test_mode 1


// help function to check if job arrive
int check_arrive(queue_t* job_queue, queue_t* ready_queue, int clock);
// helper function to dequeue the job with shortest remaning time
job_t* dequeue_short(queue_t* job_queue);


int main(int argc, char* argv[]) {


    int clock=0;
    os_srand();
    //load up the job file and do all the initilization
    // set up the whole queue thing
    queue_t* job_queue = create_queue();        // to load and store all the jobs
    queue_t* ready_queue = create_queue();      // put the job here when it arrives
    queue_t* wait_queue = create_queue();       // queue for IO waiting
    queue_t* finished_queue = create_queue();   // put finished job here

    if (test_mode){
        load_from_file("job1.txt", job_queue);
    } else {
        // Load jobs from the file
        if (argc != 2) {
            printf("Please provide one job file as an argument.\n");
            return 1;
        }
        load_from_file(argv[1], job_queue);
    }


    

    clock++;

    // While look for mimic the cpu and clock time
    job_t* current_job = NULL;

    // loop of the process
    while (!is_empty(job_queue) || !is_empty(wait_queue) || 
            !is_empty(ready_queue) || current_job != NULL) {

        if (test_mode) {
            printf("Time: %d\n", clock);
            printf("job: %d  ready: %d  wait: %d\n",
                    job_queue->count, ready_queue->count, wait_queue->count);
        }


        // check if any job in the arrive, 
        //if so choose the one with shortest remaining time
        if (check_arrive(job_queue, ready_queue, clock)) {
            enqueue(ready_queue, current_job);
            current_job = dequeue_short(ready_queue);

        }


        // check if any of the job in the wait queue has finish waiting
        int origional_count = wait_queue->count;
        for (int i = 0; i < origional_count; i++) {
            job_t* temp = dequeue(wait_queue); // pull out a job from wait
            temp->io_time++; // increment 1 to IO waiting time 
            if (IO_complete()) {

                if (test_mode) {
                    printf("IO Completed: %d\n", temp->pid);
                }

                enqueue(ready_queue, temp);
            } else {
                enqueue(wait_queue, temp); // enqueue back if still need wait
            }
        }


        // check if current job is null
        // goes to the next clock cycle if so
        if (current_job == NULL) {

            // check if there is any job left in ready
            current_job = dequeue_short(ready_queue);

            // No job is ready, keep the clock ticking
            if (current_job == NULL){
                clock++;
                continue;
            }
        }


        // check if current job is completed
        if (current_job->service_time == 0) {
            current_job->end_time = clock;  // record the end time 
            enqueue(finished_queue, current_job);  // put in finished_queue

            if (test_mode) {
                printf("A job completed: ");
                print_job(current_job);
                printf("\n");
            }

            current_job = dequeue_short(ready_queue); //swap the job to another one


            continue;
        }
        
        



        // set start time when first arrive
        if (current_job->start_time == -1) {
            current_job->start_time = clock;
        }

        // ckeck of the current job need IO
        int state = IO_request();

        if (test_mode) {
            printf("Current Job: %d  Time Left: %d\n", current_job->pid, current_job->service_time);
            printf("IO Request: %d\n", state);
            printf("\n");
        }


        if (state == 1) {
            enqueue(wait_queue, current_job);

            
            if (!is_empty(ready_queue)){
                // swap to the next job is there's more
                current_job = dequeue_short(ready_queue);
                continue;
            } else {
                // just stay at current job and waiting
                current_job = NULL;
                continue;
            }
            
        }


        

        // add 1 to ready_time to all the job in the ready queue
        node_t* temp = ready_queue->front;
        while (temp != NULL) {
            job_t* to_add = (job_t*)temp->data;
            to_add->ready_time++;
            temp = temp->next;
        }

        // increment clock time
        clock++;
        current_job->service_time--;
        current_job->cpu_time;


    }




    // need to print out everything for the finsiehd queue
    printf("        |  Total time       |  Total time       |  Total time   |\n");
    printf("Job#    |  in ready to run  |  in sleeping on   |  in system    |\n");
    printf("        |  state            |  I/O state        |               |\n");
    printf("========+===================+===================+===============+\n");
    
    // Print: iterate though finished queeu to print out result
    node_t* temp = finished_queue->front;
    int short_time = INT_MAX; //record the shortest job completion time
    int long_time = -1;       // record longest job completion time
    float total_time_comp = 0;
    float total_time_ready = 0;
    float total_time_io = 0;


    while (temp != NULL) {
        job_t* to_print = (job_t*)temp->data;
        
        // update all the numbers
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

        // print out everything
        printf("pid%4d |  %-17d|  %-17d|  %-13d|\n", 
                to_print->pid, to_print->ready_time, to_print->io_time, time_in_sys);

        // if (test_mode) {
        //     printf("pid%4d |  %-17d|  %-17d|\n", 
        //             to_print->pid, to_print->start_time, to_print->end_time);
        // }

        temp = temp->next;


    }

    // Print the total status
    printf("================================================================+\n");
    printf("Total simulation run time: %d\n", clock);
    printf("Total number of jobs: %d\n", finished_queue->count);
    printf("Shortest job completion time: %d\n", short_time);
    printf("Longest job completion time: %d\n", long_time);
	printf("Average job completion time: %.3f\n", total_time_comp / finished_queue->count);
	printf("Average time in ready queue: %.3f\n", total_time_ready / finished_queue->count);
	printf("Average time sleeping on I/O: %.3f\n", total_time_io / finished_queue->count);
    printf("\n");

    




    return 0;
}


// helper function to check if the job in job_queue has arrived
// if so move all the arrived job into the ready_queue and return 1
// return 0 if no job is ready at the current time
int check_arrive(queue_t* job_queue, queue_t* ready_queue, int clock) {

    int is_arrive = 0;
    int original_count = job_queue->count;

    for (int i = 0; i < original_count; i++) {
        job_t* temp = dequeue(job_queue); // pull out a job from job

        if (temp->arrive_time <= clock) {
            if (test_mode) {
                printf("Job arrived: %d\n", temp->pid);
            }
            is_arrive = 1;
            enqueue(ready_queue, temp);
        } else {
            enqueue(job_queue, temp); // enqueue back if still need wait
        }
    }

    return is_arrive;        

}


// helper function to dequeue the job with shortest remaning time
// if a tie happens, the one with smaller pid will be dequeue
job_t* dequeue_short(queue_t* job_queue) {

    if (job_queue->count == 0) {
        return NULL; 
    }

    job_t* short_job = NULL;
    int short_time = INT_MAX;
    int original_count = job_queue->count;


    for (int i = 0; i < original_count; i++) {
        job_t* temp = dequeue(job_queue); // pull out a job from job

        if (temp->service_time < short_time || 
           (temp->service_time == short_time && temp->pid < short_job->pid)) {
            enqueue(job_queue, short_job); // put the last short job back
            short_job = temp;
            short_time = short_job->service_time;

        } else {
            enqueue(job_queue, temp); // enqueue back 
        }
    }

    return short_job;        

}
