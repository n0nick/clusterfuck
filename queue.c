#include <stdlib.h>
#include "queue.h"

void queue_init(queue *q) {
	q->count = 0;
	q->first = 0;
}

void queue_push(queue *q, int x) {
	if (q->count == 0 && q->first == 0) {
		q->values = calloc(sizeof(int), 1);
	} else {
		q->values = (int*) realloc(q->values, q->count + q->first + 1);
	}
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
