// queue/queue.h
#ifndef QUEUE_H
#define QUEUE_H

#include "../response/response.h"

typedef enum {
    NORMAL = 0,
    URGENT = 1
} PriorityLevel;


typedef struct StudentRequest {
    int student_id;
    char* name;
    char* service_type;
    PriorityLevel priority;
    struct StudentRequest *next;  // Pointer to the next request in the queue
} StudentRequest;


typedef struct {
    StudentRequest *front;
    StudentRequest *rear;
    size_t count;
} StudentRequestQueue;


int isEmpty(const StudentRequestQueue *queue);

Response initQueue(StudentRequestQueue *queue);
Response enqueue(StudentRequestQueue *queue, StudentRequest *request);
Response dequeue(StudentRequestQueue *queue, StudentRequest *request);
Response peek(const StudentRequestQueue *queue, StudentRequest *request);
Response displayQueue(const StudentRequestQueue *queue);


#endif // QUEUE_H