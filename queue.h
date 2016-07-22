#ifndef __QUEUE_H
#define __QUEUE_H

struct queue_elem_t {
    void *data;
    
    struct queue_elem_t *next;
};

typedef struct queue_elem_t queue_elem;

struct queue_t {
    queue_elem *front;
    queue_elem *back;
};

typedef struct queue_t queue;

queue_elem *queue_elem_new(void *data);
void queue_elem_free(queue_elem *el);

queue *queue_new();
void queue_free(queue *q);
void queue_push(queue *q, queue_elem *el);
queue_elem *queue_pop(queue *q);

int queue_empty(queue *q);

#endif
