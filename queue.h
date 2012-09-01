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
    int* values;
};


void init_queue(queue *q);
void push(queue *q, int x);
int pop(queue *q);

#endif /* queueFUCK */
