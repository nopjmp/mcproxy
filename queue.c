#include "queue.h"

#include <stdlib.h>

queue_elem *queue_elem_new(void *data)
{
    queue_elem *n = malloc(sizeof(queue_elem));
    n->data = data;
    n->next = NULL;
    return n;
}

void queue_elem_free(queue_elem *el)
{
    free(el);
}

queue *queue_new()
{
    queue *q = malloc(sizeof(queue));
    q->front = NULL;
    q->back = NULL;
    return q;
}

void queue_free(queue *q)
{
    queue_elem *e = q->front;
    queue_elem *next;
    while(e != NULL) {
	next = e->next;
	queue_elem_free(e);
	e = next;
    }
    free(q);
}

void queue_push(queue *q, queue_elem *el)
{
    if (q->front == NULL && q->back == NULL) {
	q->front = el;
	q->back = el;
    } else {
	q->back->next = el;
	q->back = el;
    }
}

queue_elem *queue_pop(queue *q)
{
    queue_elem *el = q->front;

    // special case front == back
    if (q->front == q->back) {
	q->back = NULL;
    }
    
    q->front = el->next;
    return el;
}

int queue_empty(queue *q)
{
    return (q->front == NULL);
}
