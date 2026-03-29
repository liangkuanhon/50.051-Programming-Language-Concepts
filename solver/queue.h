#ifndef QUEUE_H
#define QUEUE_H

/*
 * Cell: a (x, y) coordinate in the maze grid.
 * Defined independently of maze/stack.h to keep modules decoupled.
 */
typedef struct {
    int x;
    int y;
} QCell;

/*
 * Queue: circular-buffer FIFO queue of QCell values.
 */
typedef struct {
    QCell *data;
    int    front;
    int    rear;
    int    size;
    int    capacity;
} Queue;

/*
 * create_queue: allocate a new queue with the given capacity.
 * Returns a pointer to the queue; the caller must call free_queue() when done.
 */
Queue *create_queue(int capacity);

/*
 * enqueue: add cell (x, y) to the back of the queue.
 * Behaviour is undefined if the queue is full.
 */
void enqueue(Queue *q, int x, int y);

/*
 * dequeue: remove and return the front cell.
 * Behaviour is undefined if the queue is empty.
 */
QCell dequeue(Queue *q);

/*
 * is_queue_empty: returns 1 if the queue holds no elements, 0 otherwise.
 */
int is_queue_empty(Queue *q);

/*
 * free_queue: release all memory owned by the queue.
 */
void free_queue(Queue *q);

#endif
