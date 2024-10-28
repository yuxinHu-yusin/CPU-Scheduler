// queue is added from back
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "queue.h"


// void print_process(process_t* process)
// {
//         printf("[id: %d, name: %s] ", process->id, process->name);
// }

// helper function to return the back of queue withough popping
void * peek_back(queue_t* queue) {
        if (queue->back == NULL) {
                return NULL;
        }

        void *return_element = queue->back->data;
        return return_element;
}

int is_empty(queue_t* queue) {
        int isempty = 0;
        if (queue->front == NULL && queue->back == NULL) {
                isempty = 1;
        }
        return isempty;
}

void print_queue(queue_t* queue)
{
        node_t* temp = queue->front;
        if (temp == NULL){
                printf("[Empty]");
        }

        while (temp != NULL) {
                //print_process((process_t*)temp->data);
                if (temp->next != NULL) {
                        printf("-> ");
                }
                temp = temp->next;
        }
        printf("\n");
}


queue_t *create_queue()
{
        queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
        queue->front = NULL;
        queue->back = NULL;
        queue->count = 0;
        return queue;
}


void enqueue(queue_t *queue, void *element)
{
        if (element == NULL) {
                return;
        }

        // create a node first for the element
        node_t *node = (node_t*)malloc(sizeof(node_t));
        node->next = NULL;
        node->data = element;

        if (queue->front == NULL) {
                // if there is no element
                queue->front = node;
                queue->back = node;
        } else {
                // if there is element exist, add to back
                queue->back->next = node;
                queue->back = node;

        }

        queue->count++;

}


void * dequeue(queue_t *queue)
{
        if (queue->front == NULL) {
                return NULL;
        }

        void *return_element = queue->front->data;
        node_t* to_free = queue->front;

        queue->front = queue->front->next;
        
        // set tje back to null is everything is dequeued 
        if (queue->front == NULL) {
                queue->back = NULL;
        }

        free(to_free);
        queue->count--;


        return return_element;
}


// process_t *create_process(int id, char *name)
// {
//         process_t *process = (process_t*)malloc(sizeof(process_t));
//         process->id = id;
//         process->name = (char*)malloc(strlen(name) + 1);
//         strcpy(process->name, name);
//         return process;
// }


// void free_process(process_t *process)
// {
//         if (process) {
//                 free(process->name);
//                 free(process);
//         }
// }


void free_queue(queue_t* queue)
{
        node_t* temp = queue->front;
        while (temp != NULL) {
                free(temp->data);
                temp = temp->next;
        }

        free(queue);
}
