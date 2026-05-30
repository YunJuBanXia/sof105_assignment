// queue/queue.c
#include "queue.h"

#include <stddef.h>
#include <stdio.h>
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
    // Validate input
    if (queue == NULL || request == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid input provided.");
    }

    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode));

    if (new_node == NULL) {
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to allocate memory for new request.");
    }
    new_node->request = *request;  // Copy the request data
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

    QueueNode* temp = queue->front;
    *request = temp->request;  // Copy the request data

    queue->front = queue->front->next;
    free(temp);
    queue->count--;

    return makeResponse(SUCCESS, "Request removed from queue successfully.");
}


Response peek(const StudentRequestQueue *queue, StudentRequest *request) {  // The request parameter is used to return the front request
    if (isEmpty(queue)) {
        return makeResponse(ERROR_QUEUE_EMPTY, "Queue is empty.");
    }

    *request = queue->front->request;  // Copy the request data

    return makeResponse(SUCCESS, "Request peeked successfully.");
}


Response displayQueue(const StudentRequestQueue *queue) {
    if (isEmpty(queue)) {
        return makeResponse(ERROR_QUEUE_EMPTY, "Queue is empty.");
    }

    QueueNode *current = queue->front;

    int max_name_length = 10;  // Minimum width for name column
    int max_service_length = 12;  // Minimum width for service type column
    // First pass to determine the maximum width of name and service
    while (current != NULL) {
        int name_length = snprintf(NULL, 0, "%s", current->request.name);
        int service_length = snprintf(NULL, 0, "%s", current->request.service_type);
        if (name_length > max_name_length) {
            max_name_length = name_length;
        }
        if (service_length > max_service_length) {
            max_service_length = service_length;
        }
        current = current->next;
    }

    // Second pass to print the queue
    printf("\nWaiting Queue:\n");
    current = queue->front;
    int count = 1;
    while (current != NULL) {
        printf("\t%d. %d  %-*s  %-*s  %-10s\n",
            count,
            current->request.student_id,
            max_name_length, current->request.name,
            max_service_length, current->request.service_type,
            current->request.priority == NORMAL ? "Normal" : "Urgent"
        );
        count++;
        current = current->next;
    }

    return makeResponse(SUCCESS, "Queue displayed successfully.");
}