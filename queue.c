#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

queue_t* create_queue() {
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    queue->front = queue->back = NULL;
    queue->count = 0;
    return queue;
}

void enqueue(queue_t* queue, void* element) {
    node_t* node = (node_t*)malloc(sizeof(node_t));
    node->data = element;
    node->next = NULL;
    if (queue->back != NULL) {
        queue->back->next = node;
    }
    queue->back = node;
    if (queue->front == NULL) {
        queue->front = node;
    }
    queue->count++;
}

void* dequeue(queue_t* queue) {
    if (queue->front == NULL) return NULL;
    node_t* node = queue->front;
    void* element = node->data;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->back = NULL;
    }
    free(node);
    queue->count--;
    return element;
}

int is_empty(queue_t* queue) {
    return queue->count == 0;
}

void free_queue(queue_t* queue) {
    while (!is_empty(queue)) {
        dequeue(queue);
    }
    free(queue);
}
