#include "queue.h"

void QueueInit(xQueue *Q, uint8_t *buf, int bufsize)
{
    Q->buf = buf;
    Q->bufsize = bufsize;
    Q->front = 0;
    Q->rear = 0;
    Q->size = 0;
}

bool EnQueue(xQueue *Q, uint8_t ch)
{
    int next = (Q->rear + 1) % Q->bufsize;      //下一个要写的位置

    if(next == Q->front)      //队列已满
    {
        return false;
    }

    Q->buf[Q->rear] = ch;
    Q->rear = next;
    return true;
}

uint8_t DeQueue(xQueue *Q)
{
    if(Q->rear == Q->front)      //队列为空
    {
        return '\0';
    }

    uint8_t ch = Q->buf[Q->front];
    Q->front = (Q->front + 1) % Q->bufsize;     //下一个出队位置

    return ch;
}

void QueueClean(xQueue *Q)
{
    Q->rear = Q->front = 0;
}

uint16_t QueueLen(xQueue *Q)
{
    return (Q->rear - Q->front + Q->bufsize) % Q->bufsize;
}
