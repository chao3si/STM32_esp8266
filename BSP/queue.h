#ifndef __QUEUE_H
#define __QUEUE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Queue
{
    uint8_t *buf;
    int bufsize;
    int front;
    int rear;
    int size;
} xQueue;

void QueueInit(xQueue *Q, uint8_t *buf, int bufsize);
bool EnQueue(xQueue *Q, uint8_t ch);
uint8_t DeQueue(xQueue *Q);
void QueueClean(xQueue *Q);
uint16_t QueueLen(xQueue *Q);

#endif
