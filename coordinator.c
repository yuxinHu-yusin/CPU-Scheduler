// coordinator.c
#include <stdio.h>
#include <limits.h>
#include "job.c"

int main(int argc, char* argv[]) {

    int test_mode = 1; 

    int clock=0;
    os_srand();
    //load up the job file and do all the initilization
    // set up the whole queue thing
    queue_t* job_queue = create_queue();
    queue_t* wait_queue = create_queue();
    queue_t* finished_queue = create_queue();

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


    // just demo, job follow fifo, no scheduling
    while (!is_empty(job_queue) || !is_empty(wait_queue)) {

        // dequeue to get current job
        job_t* current_job = dequeue(job_queue);

        // printf("Job dequeued - PID: %d, Arrival: %d, Service: %d, Priority: %d\n",
        //       current_job->pid, current_job->arrive_time, current_job->service_time, current_job->priority);
    
        // loop of the process
        while (1) {

            // set start time when first arrive
            if (current_job->start_time == -1) {
                current_job->start_time = clock;
            }

            // ckeck of the current job need IO
            int state = IO_request();

            if (test_mode) {
                printf("Time: %d\n", clock);
                printf("Current Job: %d  Time Left: %d\n", current_job->pid, current_job->service_time);
                printf("IO Request: %d\n", state);
                printf("\n");

            
            }


            if (state == 1) {
                enqueue(wait_queue, current_job);

                
                if (!is_empty(job_queue)){
                    // swap to the next job is there's more
                    current_job = dequeue(job_queue);
                    continue;
                } else {
                    // just stay at current job and waiting
                    current_job = NULL;
                    continue;
                }
                
            }

            // check if any of the job in the wait queue has finish waiting
            for (int i = 0; i < wait_queue->count; i++) {
                job_t* temp = dequeue(wait_queue); // pull out a job from wait
                temp->io_time++; // increment 1 to IO waiting time 
                if (IO_complete()) {

                    if (test_mode) {
                        printf("IO Completed: %d\n", temp->pid);
                    }

                    enqueue(job_queue, temp);
                } else {
                    enqueue(wait_queue, temp); // enqueue back if still need wait
                }
            }
            


            // check if current job is completed
            if (current_job->service_time == 0) {
                // record the end time when a job is completed
                current_job->end_time = clock;
                enqueue(finished_queue, current_job); 
                if (test_mode) {
                    printf("A job completed: ");
                    print_job(current_job);
                    printf("\n");
                }


                break;
            }
            
            // add 1 to ready_time to all the job in the ready queue
            node_t* temp = job_queue->front;
            while (temp != NULL) {
                job_t* to_add = (job_t*)temp->data;
                to_add->ready_time++;
                temp = temp->next;
            }
    
            // increment clock time
            clock++;
            current_job->service_time--;

        }


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
    int total_time_comp = 0;
    int total_time_ready = 0;
    int total_time_io = 0;


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
    printf("Average job completion time: %d\n", total_time_comp / finished_queue->count);
    printf("Average time in ready queue: %d\n", total_time_ready / finished_queue->count);
    printf("Average time sleeping on I/O: %d\n", total_time_io / finished_queue->count);
    printf("\n");

    




    return 0;
}