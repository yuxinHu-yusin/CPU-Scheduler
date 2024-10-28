#ifndef QUEUE_H
#define QUEUE_H

typedef struct node {
    void *data;
    struct node* next;
} node_t;

typedef struct {
    node_t *front;
    node_t *back;
    int count;
} queue_t;

queue_t* create_queue();
void enqueue(queue_t *queue, void *element);
void* dequeue(queue_t *queue);
int is_empty(queue_t *queue);
void free_queue(queue_t* queue);

#endif
