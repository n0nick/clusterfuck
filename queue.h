/*
 * queue.h
 *
 *  Created on: Sep 1, 2012
 *      Author: sagiemao
 */

#ifndef queueFUCK
#define queueFUCK

typedef struct queue queue;

struct queue {
    int count;
    int first;
    int* values;
};


void queue_init(queue *q);
void queue_push(queue *q, int x);
int queue_pop(queue *q);
void queue_free(queue *q);

#endif /* queueFUCK */
