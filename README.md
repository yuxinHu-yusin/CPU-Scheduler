**CPU Scheduling Algorithms in C**

**Group 6 Team Member:**
Jingyi Chen, Yi-Hsuan Lai, Yuxin Hu


**Project Overview**<br />
This project demonstrates the implementation of three CPU scheduling algorithms in C. It includes code for Round Robin (RR), Shortest Remaining Time First (SRTF), and Multilevel Feedback Queue (MLFQ) scheduling, with necessary utilities such as process/job management.<br />

**Round Robin (RR):** Each process is given a fixed time slice in a cyclic manner.<br />

**Shortest Remaining Time First (SRTF):** Preemptive scheduling that selects the process with the shortest remaining time to completion for execution.<br />

**Multilevel Feedback Queue (MLFQ):** A more dynamic approach where processes move between queues based on their execution behavior.<br />

	Rule1: If Priority(A) > Priority(B), A runs (B does not)
	Rule2: If Priority(A) = Priority(B), A and B run in Round-Robin
	Rule3: When a job enter the system, it is placed at the highest priority queue
	Rule4: Once a job use up its time allotment at a given level, its priority is reduced
	Rule5: After some time period S, move all the job in the system to the topmost queue

**Key Assumptions**<br />
**Time Slice:** In RR, each process receives a fixed time slice of 2 units of time before it is preempted.<br />
**Preemption:**  Both SRTF and MLFQ can preempt the currently running process to schedule another process.<br />
**Process Arrival:** All algorithms assume that processes arrive at predefined intervals through job configuration files (e.g., input.txt).<br />
**Job Data:** All processes/jobs are predefined with attributes such as arrival time, burst time, and priority.<br />

**Work Distribution**<br />
Yi-Hsuan Lai<br />
	&emsp;Developed SRTF.c and verified the correctness of preemption logic.<br />
	&emsp;Created queue.c and queue.h to implement the queue structure used by all scheduling algorithms.<br />
	&emsp;Created job.c and job.h to designed I/O operations and random number handling to simulate I/O requests and completions.<br />
Yuxin Hu<br />
	&emsp;Created job.c and job.h to handle process details and settings.<br />
	&emsp;Implemented RR.c, ensuring correct time quantum handling and process cycling.<br />
Jingyi Chen<br />
	&emsp;Developed MLFQ.c, including code to manage queues and adjust priorities.<br />
	&emsp;Created the Makefile to automate the build process.<br />

**Description of Files**<br />
&ensp;**Makefile:** Automates compilation and builds the project using the provided C files.<br />
&ensp;**MLFQ.c**: Implements the Multilevel Feedback Queue scheduling algorithm.<br />
&ensp;**RR.c:** Implements the Round Robin scheduling algorithm.<br />
&ensp;**SRTF.c**: Implements the Shortest Remaining Time First scheduling algorithm.<br />
&ensp;**queue.c:** Defines the queue structure and associated functions for process management.<br />
&ensp;**job.c and job.h:** Contains logic for managing processes (or jobs) with relevant structures and operations.<br />
&ensp;**input1.txt and input2.txt:** Contains attributes such as arrival time, burst time, and priority.<br />

**How to use Makefile:** <br />

	1.To compile all three programs (RR, MLFQ, and SRTF), simply run:
		make
	2. If you want to compile only one specific program, simply run:
		make MLFQ
		make RR
		make SRTF
	3. Run test: 
		make test
make test: This command will run each executable (RR, MLFQ, SRTF) with test input files job1.txt and job2.txt. The output will be saved in files like RR_output1.txt, MLFQ_output1.txt, etc.

**Algorithm Testing and Validation**<br />
Shortest Remaining Time First (SRTF)<br />
Round Robin (RR)<br />
Multilevel Feedback Queue (MLFQ)<br />



