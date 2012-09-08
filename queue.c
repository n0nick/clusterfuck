#include <stdlib.h>
#include "queue.h"
#include <stdio.h>

void queue_init(queue *q) {
	q->count = 0;
	q->first = 0;
	q->values = NULL;
}

void queue_push(queue *q, int x) {
	q->values = (int*) realloc(q->values, sizeof(int) * (q->count + q->first + 1));
	q->values[q->count + q->first] = x;
	q->count++;
}

int queue_pop(queue *q) {
	int x = q->values[q->first];

	q->first++;
	q->count--;
	return x;
}

void queue_free(queue *q) {
	free(q->values);
}
