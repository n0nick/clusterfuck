#include <stdlib.h>
#include "queue.h"

void init_queue(queue *q) {
	q->count = 0;
}

void push(queue *q, int x) {
	q->count++;
	q->values = (int*) realloc(q->values, q->count);
	q->values[q->count - 1] = x;
}

int pop(queue *q) {
	int x = q->values[0];

	/*TODO memory leak?! */
	q->values = (q->values) + sizeof(int);
	q->count--;
	return x;
}
