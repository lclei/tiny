#ifndef SBUF_H_
#define SBUF_H_
#include "mynet.h"
#include <semaphore.h>

typedef struct {
	int *buf;
	int n;
	int front;
	int rear;
	sem_t mutex;
	sem_t slots;
	sem_t items;
}sbuf_t;

void dbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *p);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);
#endif
