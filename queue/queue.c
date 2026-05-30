// queue/queue.c
#include "queue.h"

#include <stddef.h>
#include <stdlib.h>


int isEmpty(const StudentRequestQueue *queue) {
    return queue->count == 0;
}


Response initQueue(StudentRequestQueue *queue) {
    queue->front = NULL;
    queue->rear = NULL;
    queue->count = 0;
    return makeResponse(SUCCESS, "Queue initialized successfully.");
}


Response enqueue(StudentRequestQueue *queue, StudentRequest *request) {
    // Validate if input is valid
    if (queue == NULL || request == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid input provided.");
    }

    StudentRequest* new_node = (StudentRequest*)malloc(sizeof(StudentRequest));

    if (new_node == NULL) {
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to allocate memory for new request.");
    }
    *new_node = *request;  // Copy the request data
    new_node->next = NULL; // Ensure the next pointer is null

    // Add the new node to the queue
    if (isEmpty(queue)) {
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    queue->count++;

    return makeResponse(SUCCESS, "Request added to queue successfully.");
}


Response dequeue(StudentRequestQueue *queue, StudentRequest *request) {  // The request parameter is used to return the removed request
    if (isEmpty(queue)) {
        return makeResponse(ERROR_QUEUE_EMPTY, "Queue is empty.");
    }

    StudentRequest* temp = queue->front;
    *request = *temp;  // Copy the request data

    queue->front = queue->front->next;
    free(temp);
    queue->count--;

    return makeResponse(SUCCESS, "Request removed from queue successfully.");
}


Response peek(const StudentRequestQueue *queue, StudentRequest *request) {  // The request parameter is used to return the front request
    if (isEmpty(queue)) {
        return makeResponse(ERROR_QUEUE_EMPTY, "Queue is empty.");
    }

    *request = *(queue->front);  // Copy the request data

    return makeResponse(SUCCESS, "Request peeked successfully.");
}