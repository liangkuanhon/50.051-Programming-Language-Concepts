#include <stdlib.h>
#include "queue.h"

Queue *create_queue(int capacity) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->data     = (QCell *)malloc(capacity * sizeof(QCell));
    q->front    = 0;
    q->rear     = 0;
    q->size     = 0;
    q->capacity = capacity;
    return q;
}

void enqueue(Queue *q, int x, int y) {
    q->data[q->rear].x = x;
    q->data[q->rear].y = y;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;
}

QCell dequeue(Queue *q) {
    QCell cell = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return cell;
}

int is_queue_empty(Queue *q) {
    return q->size == 0;
}

void free_queue(Queue *q) {
    free(q->data);
    free(q);
}
