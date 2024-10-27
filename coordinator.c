// coordinator.c
#include <stdio.h>
#include "job.c"

int main(int argc, char* argv[]) {


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
    while (!is_empty(job_queue)) {

        // dequeue to get current job
        job_t* current_job = dequeue(job_queue);

        // printf("Job dequeued - PID: %d, Arrival: %d, Service: %d, Priority: %d\n",
        //       current_job->pid, current_job->arrive_time, current_job->service_time, current_job->priority);
    
        // loop of the process
        while (1) {


            int state = IO_request;
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

            if (current_job->service_time == 0) {
                printf("\nA job completed");
                print_job(current_job);
                break;
            }
            
            
            clock++;
            current_job->service_time--;

        }


        
    
    }



    return 0;
}