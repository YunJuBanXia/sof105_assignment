// queue/queue.h
#ifndef QUEUE_H
#define QUEUE_H

#include <stddef.h>

#include "../response/response.h"
#include "../student.h"


typedef struct QueueNode {
    StudentRequest request;
    struct QueueNode* next;
} QueueNode;


typedef struct {
    QueueNode *front;
    QueueNode *rear;
    size_t count;
} StudentRequestQueue;


Response isEmptyQueue(const StudentRequestQueue *queue, int *result);  // result is used to return whether the queue is empty (1 for empty, 0 for not empty)
Response initQueue(StudentRequestQueue *queue);
Response enqueue(StudentRequestQueue *queue, StudentRequest *request);
Response dequeue(StudentRequestQueue *queue, StudentRequest *request);  // request parameter is used to return the removed request
Response peekQueue(const StudentRequestQueue *queue, StudentRequest *request);  // request parameter is used to return the front request without removing it
Response displayQueue(const StudentRequestQueue *queue);


#endif // QUEUE_H