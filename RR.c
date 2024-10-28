// coordinator.c
#include "job.c"
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

#define test_mode 1

// define the time quantum as 2
#define TIME_QUANTUM 2

int check_arrive(queue_t* job_queue, queue_t* ready_queue, int clock);

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
        // load jobs from the file
        if (argc != 2) {
            printf("Please provide one job file as an argument.\n");
            return 1;
        }
        load_from_file(argv[1], job_queue);
    }

    clock++;

    // RR scheduler
    while(!is_empty(ready_queue) || !is_empty(wait_queue) || !is_empty(job_queue)){

        // check if any job in the arrive, 
        check_arrive(job_queue, ready_queue, clock);

    	// we first check if there are job competed I/O 
    	int io_count = wait_queue->count;
    	
    	for(int i = 0; i < io_count; i++){
    		job_t* io_job = dequeue(wait_queue);
    		io_job->io_time++; 

    		// if complete, put it into the ready queue
    		if (IO_complete()) {
                printf("I/O completed for Job %d\n", io_job->pid);
                enqueue(ready_queue, io_job);  // Move back to ready queue
            } else {
                enqueue(wait_queue, io_job);  // Still waiting, put back to waiting queue
            }
    	}

    	if (test_mode) {
                printf("Time: %d\n", clock);
                printf("job: %d  ready: %d  wait: %d\n",
                        job_queue->count, ready_queue->count, wait_queue->count);

            }
            

    	// if there are jobs in the ready queue
    	if(!is_empty(ready_queue)){
    		job_t *curr_job = dequeue(ready_queue);

    		// Track the start time if this is the first time the job runs
            if (curr_job->start_time == -1) {
                curr_job->start_time = clock;  // Set start time to current clock
                
            }

    		// update the remain time need
    		int run_t = 0;
    		if(curr_job->service_time > TIME_QUANTUM){
    			run_t = TIME_QUANTUM;
    		}else{
    			run_t = curr_job->service_time;
    		}

            // Simulate execution: increment clock and check for new arrivals at each tick
            for (int i = 0; i < run_t; i++) {
                // Increment the clock by 1 unit
                clock++;  

                // check if new job arrival during each tick
                check_arrive(job_queue, ready_queue, clock);

                // increment ready_time for all waiting jobs in the ready queue
                node_t *temp = ready_queue->front;
                while (temp != NULL) {
                    job_t *waiting_job = (job_t *)temp->data;
                    // increase by 1 per clock tick
                    waiting_job->ready_time = waiting_job->ready_time + 1;  
                    temp = temp->next;  

                }
            }

            // update the job’s remaining service time
            curr_job->service_time -= run_t;
    		

    		// check if the job request I/O
    		if (IO_request()) {
                printf("Job %d requested I/O, moving to I/O queue\n", curr_job->pid);
                enqueue(wait_queue, curr_job);  // Move to I/O queue
            }else if (curr_job->service_time == 0) {
            	// check if the job finished
		     	curr_job->end_time = clock;  // Record the end time
		     	curr_job->completed = 1;
		        printf("Job %d completed and moved to finished queue.\n", curr_job->pid);
		        enqueue(finished_queue, curr_job);  // Move to finished queue
		    } else {
		        // If the job is not complete and do not need I/O
		        enqueue(ready_queue, curr_job);
		    }
		    
		}else {
        // if no jobs are ready, increment the clock to simulate idle time
        	clock += 1;
        }
	}

  	// need to print out everything for the finsiehd queue
    printf("        |  Total time       |  Total time       |  Total time   |\n");
    printf("Job#    |  in ready to run  |  in sleeping on   |  in system    |\n");
    printf("        |  state            |  I/O state        |               |\n");
    printf("========+===================+===================+===============+\n");
    
    // Print: iterate though finished queeu to print out result
    node_t* temp = finished_queue->front;
    float total_time_comp = 0;
    float total_time_ready = 0;
    float total_time_io = 0;


    while (temp != NULL) {
        job_t* to_print = (job_t*)temp->data;
        
        // update all the numbers
        int time_in_sys = to_print->end_time - to_print->arrive_time;
        total_time_comp += time_in_sys;
        total_time_ready += to_print->ready_time;
        total_time_io += to_print->io_time;
        // print out everything
        printf("pid%4d |  %-17d|  %-17d|  %-13d|\n", 
                to_print->pid, to_print->ready_time, to_print->io_time, time_in_sys);

        temp = temp->next;
    }

    // Print the total status
    printf("================================================================+\n");
    printf("Total simulation run time: %d\n", clock);
    printf("Total number of jobs: %d\n", finished_queue->count);
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









