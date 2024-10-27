// coordinator.c
#include <stdio.h>
#include "job.c"

int main(int argc, char* argv[]) {

    int test_mode = 1; 

    int clock=0;
    os_srand();
    //load up the job file and do all the initilization
    // set up the whole queue thing
    queue_t* job_queue = create_queue();
    queue_t* wait_queue = create_queue();

    // Load jobs from the file
    if (argc != 2) {
        printf("Please provide one job file as an argument.\n");
        return 1;
    }
    load_from_file(argv[1], job_queue);

    

    clock++;


    // just demo, job follow fifo, no scheduling
    while (!is_empty(job_queue) || !is_empty(wait_queue)) {

        // dequeue to get current job
        job_t* current_job = dequeue(job_queue);

        // printf("Job dequeued - PID: %d, Arrival: %d, Service: %d, Priority: %d\n",
        //       current_job->pid, current_job->arrive_time, current_job->service_time, current_job->priority);
    
        // loop of the process
        while (1) {

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
                if (IO_complete()) {
                    enqueue(job_queue, temp);
                } else {
                    enqueue(wait_queue, temp); // enqueue back if still need wait
                }
            }
            



            if (current_job->service_time == 0) {
                printf("A job completed: ");
                print_job(current_job);
                printf("\n");

                break;
            }
            
            
            clock++;
            current_job->service_time--;

        }


        
    
    }



    return 0;
}