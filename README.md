# CPU Scheduling Algorithms in C

### Group 6 Team Members:
- Jingyi Chen
- Yi-Hsuan Lai
- Yuxin Hu


## Project Overview
This project demonstrates the implementation of three CPU scheduling algorithms in C. It includes code for Round Robin (RR), Shortest Remaining Time First (SRTF), and Multilevel Feedback Queue (MLFQ) scheduling, with necessary utilities such as process/job management.<br />

### Scheduling Algorithms
- **Round Robin (RR):** Each process is given a fixed time slice in a cyclic manner.<br />

- **Shortest Remaining Time First (SRTF):** Preemptive scheduling that selects the process with the shortest remaining time to completion for execution.<br />

- **Multilevel Feedback Queue (MLFQ):** A more dynamic approach where processes move between queues based on their execution behavior.<br />

	**MLFQ Rules:**
	1. If Priority(A) > Priority(B), A runs (B does not)
	2. If Priority(A) = Priority(B), A and B run in Round-Robin
	3. When a job enter the system, it is placed at the highest priority queue
	4. Once a job use up its time allotment at a given level, its priority is reduced
	5. After some time period S, move all the job in the system to the topmost queue

## Key Assumptions<br />
- **Time Slice:** In RR, each process receives a fixed time slice of 2 units of time before it is preempted.<br />
- **Preemption:**  Both SRTF and MLFQ can preempt the currently running process to schedule another process.<br />
- **Process Arrival:** All algorithms assume that processes arrive at predefined intervals through job configuration files (e.g., input.txt).<br />
- **Job Data:** 
  - All processes/jobs are predefined with attributes such as arrival time, burst time, and priority.<br />
  - Loading the input file is assumed to takes up one clock cycle.
- **I/O Requests**:
  - I/O requests and acquisitions occur randomly.
  - Multiple jobs may complete their I/O wait within a single clock cycle.
  - The need for I/O is checked at each time slice in most algorithms, except for SRTF, where it is checked at each clock cycle.
  - A job that has acquired I/O may need additional I/O requests later in its runtime, depending on process requirements.<br />

## Work Distribution
- **Yi-Hsuan Lai**
  - Developed `SRTF.c` and verified the correctness of preemption logic.
  - Created `queue.c` and `queue.h` to implement the queue structure used by all scheduling algorithms.
  - Designed I/O operations and random handling in `job.c` and `job.h` to simulate I/O requests and completions.

- **Yuxin Hu**
  - Handled process details and settings in `job.c` and `job.h`.
  - Implemented `RR.c`, ensuring correct time quantum handling and process cycling.

- **Jingyi Chen**
  - Developed `MLFQ.c`, including code to manage queues and adjust priorities.
  - Created the `Makefile` to automate the build process.


## Description of Files<br />
- **Makefile:** Automates compilation and builds the project using the provided C files.
    1. To compile all three programs (RR, MLFQ, and SRTF), simply run:
		make
	2. If you want to compile only one specific program, simply run:
		make MLFQ
		make RR
		make SRTF
	3. Run test: 
		make test
This command will run each executable (RR, MLFQ, SRTF) with test input files job1.txt and job2.txt. The output will be saved in files like RR_output1.txt, MLFQ_output1.txt, etc.
- &ensp;**MLFQ.c**: Implements the Multilevel Feedback Queue scheduling algorithm.<br />
- &ensp;**RR.c:** Implements the Round Robin scheduling algorithm.<br />
- &ensp;**SRTF.c**: Implements the Shortest Remaining Time First scheduling algorithm.<br />
- &ensp;**queue.c:** Defines the queue structure and associated functions for process management.<br />
- &ensp;**job.c and job.h:** Contains logic for managing processes (or jobs) with relevant structures and operations.<br />
- &ensp;**input1.txt and input2.txt:** Contains attributes such as arrival time, burst time, and priority.<br />


## Algorithm Testing and Validation<br />
A test mode is implemented for this algorithm to print out key information on the simulated CPU’s state at each clock cycle. This includes details such as the current job running, the elapsed time, and notifications when a job arrives, completes, requests I/O, or acquires I/O. Additionally, it displays the number of jobs in each queue. This test mode allows us to observe the behavior of the simulated CPU and jobs, ensuring they align with our scheduling model.Our group also manually calculated some outputs using the same input and observed the I/O requesting/waiting state to verify results.`SRTF_output1_dryrun.txt` is the sample output file of `job1.txt` using the test mode and the result is verified that the matches the schdualing rule.

- **Shortest Remaining Time First (SRTF)** Observations show that the job with the shortest remaining time is consistently prioritized. Whenever a job finishes or a new job enters the ready queue (due to arrival or after completing I/O), the scheduler re-evaluates and swaps in the job with the shortest remaining time. We also note that longer jobs tend to experience longer times in the ready queue, as the scheduler is biased towards shorter jobs, which are processed earlier, leaving longer jobs toward the end.
- **Round Robin (RR)** The Round-Robin (RR) scheduling algorithm in the code organizes and processes jobs in a fair, time-sliced manner. It begins by initializing four queues:
  - **job_queue** for jobs that haven’t arrived yet.
  - **ready_queue** for jobs ready to run.
  - **wait_queue** for jobs waiting on I/O.
  - **finish_queue** for completed jobs.

   The algorithm uses a clock to simulate time. Each cycle, it checks if any jobs in the job_queue have reached their arrival time and moves them to the ready_queue Jobs waiting on I/O are also checked; if their I/O completes, they return to the ready_queue. The first job in the ready_queue is given up to 2 time units (the time quantum) to run. If the job’s time is up but it isn’t complete, it’s re-added to the ready_queue to continue later. If the job needs I/O, it’s moved to the wait_queue; if it’s finished, it’s sent to the finish_queue. The algorithm continues these steps until all jobs are either completed or waiting in the I/O queue. Finally, it outputs statistics for each job, including total time in the system, waiting time, and I/O time. It is observed that each job shares a more even number of total time in ready to run state compared to SRTF.

- **Multilevel Feedback Queue (MLFQ)**
Observations show that the MLFQ scheduler prioritizes jobs based on dynamic feedback from their runtime behavior. Jobs start in the highest priority queue, and if they exceed the time quantum, they are demoted to a lower priority level. This setup allows shorter jobs to complete quickly, while longer jobs progressively move down the queues. The boost mechanism periodically promotes all jobs to the highest-priority queue, preventing starvation and ensuring fair allocation of CPU time across all jobs. The MLFQ's adaptive nature enables it to handle a variety of workloads effectively, balancing responsiveness for short tasks with fairness for longer tasks. In test mode, we observed that jobs with frequent I/O requests tend to remain in the higher-priority queues due to their short CPU bursts, while CPU-intensive jobs gradually move to lower levels, reflecting the scheduler's design to optimize for interactivity and efficiency.



