#include <stdio.h>
#include <stdlib.h>
#include "job.h"
#include "queue.h"  // 确保包含了 queue.h

#define TIME_QUANTUM 2
#define TEST_MODE 1

int check_arrive(queue_t* job_queue, queue_t* ready_queue, int clock);

int main(int argc, char* argv[]) {
    int clock = 0;
    os_srand();  // 初始化随机数生成器

    queue_t* job_queue = create_queue();
    queue_t* ready_queue = create_queue();
    queue_t* wait_queue = create_queue();
    queue_t* finished_queue = create_queue();

    if (argc == 2) {
        if (load_from_file(argv[1], job_queue) == -1) {
            fprintf(stderr, "Error: Failed to open file %s\n", argv[1]);
            return 1;
        }
    } else {
        fprintf(stderr, "Usage: %s <job_file>\n", argv[0]);
        return 1;
    }

    clock++;

    // Round Robin 调度器主循环
    while (!is_empty(ready_queue) || !is_empty(wait_queue) || !is_empty(job_queue)) {
        check_arrive(job_queue, ready_queue, clock);

        // 处理等待队列中的作业
        int io_count = wait_queue->count;
        for (int i = 0; i < io_count; i++) {
            job_t* io_job = dequeue(wait_queue);
            io_job->io_time++;

            if (IO_complete()) {
                printf("I/O completed for Job %d\n", io_job->pid);
                enqueue(ready_queue, io_job);
            } else {
                enqueue(wait_queue, io_job);
            }
        }

        // 更新并处理就绪队列中的作业
        if (!is_empty(ready_queue)) {
            job_t *curr_job = dequeue(ready_queue);

            if (curr_job->start_time == -1) {
                curr_job->start_time = clock;
            }

            int run_time = (curr_job->service_time > TIME_QUANTUM) ? TIME_QUANTUM : curr_job->service_time;

            // 模拟作业运行
            for (int t = 0; t < run_time; t++) {
                curr_job->service_time--;
                curr_job->cpu_time++;
                clock++;

                // 更新就绪队列中其他作业的 ready_time
                node_t* temp_node = ready_queue->front;
                while (temp_node != NULL) {
                    job_t* temp_job = (job_t*)temp_node->data;
                    temp_job->ready_time++;
                    temp_node = temp_node->next;
                }

                // 更新等待队列中作业的 io_time
                node_t* io_node = wait_queue->front;
                while (io_node != NULL) {
                    job_t* io_job = (job_t*)io_node->data;
                    io_job->io_time++;
                    io_node = io_node->next;
                }

                check_arrive(job_queue, ready_queue, clock);

                // 检查是否发生 I/O 请求
                if (IO_request()) {
                    printf("Job %d requested I/O, moving to I/O queue\n", curr_job->pid);
                    enqueue(wait_queue, curr_job);
                    curr_job = NULL;
                    break;
                }

                // 如果作业在时间片内完成
                if (curr_job->service_time == 0) {
                    curr_job->end_time = clock;
                    curr_job->completed = 1;
                    printf("Job %d completed and moved to finished queue.\n", curr_job->pid);
                    enqueue(finished_queue, curr_job);
                    curr_job = NULL;
                    break;
                }
            }

            // 如果作业未完成且未请求 I/O，将其放回就绪队列
            if (curr_job != NULL && curr_job->service_time > 0) {
                enqueue(ready_queue, curr_job);
            }

        } else {
            // 如果没有作业就绪，推进时钟
            clock++;
        }
    }

    // 打印统计信息
    printf("        |  Total time       |  Total time       |  Total time   |\n");
    printf("Job#    |  in ready to run  |  in sleeping on   |  in system    |\n");
    printf("        |  state            |  I/O state        |               |\n");
    printf("========+===================+===================+===============+\n");

    // 汇总统计数据
    node_t* temp = finished_queue->front;
    float total_time_comp = 0;
    float total_time_ready = 0;
    float total_time_io = 0;

    while (temp != NULL) {
        job_t* to_print = (job_t*)temp->data;

        int time_in_sys = to_print->end_time - to_print->arrive_time; // 在系统中的总时间
        total_time_comp += time_in_sys;
        total_time_ready += to_print->ready_time;
        total_time_io += to_print->io_time;

        printf("pid%4d |  %-17d|  %-17d|  %-13d|\n", to_print->pid, to_print->ready_time, to_print->io_time, time_in_sys);
        temp = temp->next;
    }

    int total_jobs = finished_queue->count;
    printf("================================================================+\n");
    printf("Total simulation run time: %d\n", clock);
    printf("Total number of jobs: %d\n", total_jobs);
    printf("Average job completion time: %.3f\n", total_time_comp / total_jobs);
    printf("Average time in ready queue: %.3f\n", total_time_ready / total_jobs);
    printf("Average time sleeping on I/O: %.3f\n", total_time_io / total_jobs);
    printf("\n");

    // 释放内存
    free_queue(job_queue);
    free_queue(ready_queue);
    free_queue(wait_queue);
    free_queue(finished_queue);

    return 0;
}

int check_arrive(queue_t* job_queue, queue_t* ready_queue, int clock) {
    int is_arrive = 0;
    int original_count = job_queue->count;

    for (int i = 0; i < original_count; i++) {
        job_t* temp = dequeue(job_queue);
        if (temp->arrive_time <= clock) {
            is_arrive = 1;
            enqueue(ready_queue, temp);
        } else {
            enqueue(job_queue, temp);
        }
    }

    return is_arrive;
}
