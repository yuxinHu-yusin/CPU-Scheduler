CC = gcc
CFLAGS = -Wall -g

EXECS = RR MLFQ SRTF

RR_SRCS = coordinator_RR.c job.c queue.c
MLFQ_SRCS = coordinator_MLFQ.c job.c queue.c
SRTF_SRCS = coordinator_SRTF.c job.c queue.c

all: $(EXECS)

RR: $(RR_SRCS)
	$(CC) $(CFLAGS) -o RR $(RR_SRCS)

MLFQ: $(MLFQ_SRCS)
	$(CC) $(CFLAGS) -o MLFQ $(MLFQ_SRCS)

SRTF: $(SRTF_SRCS)
	$(CC) $(CFLAGS) -o SRTF $(SRTF_SRCS)

test: all
	./RR input1.txt > RR_output1.txt
	./RR input2.txt > RR_output2.txt
	./RR input3.txt > RR_output3.txt
	./MLFQ input1.txt > MLFQ_output1.txt
	./MLFQ input2.txt > MLFQ_output2.txt
	./MLFQ input3.txt > MLFQ_output3.txt
	./SRTF input1.txt > SRTF_output1.txt
	./SRTF input2.txt > SRTF_output2.txt
	./SRTF input3.txt > SRTF_output3.txt

	echo "Tests completed. Check output files."

clean:
	rm -f $(EXECS) *.o *_output1.txt *_output2.txt *_output3.text *.dsYM
