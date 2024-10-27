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

int is_empty(queue_t *queue);
void enqueue(queue_t *queue, void *element);
void * dequeue(queue_t *queue);
queue_t *create_queue();
void print_process();
void print_queue();
void free_queue();

#endif