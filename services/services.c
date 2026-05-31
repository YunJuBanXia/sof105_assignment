// services/services.c
#include "services.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static char *duplicateString(const char *source) {
    if (source == NULL) {
        return NULL;
    }

    size_t length = strlen(source) + 1;
    char *copy = (char *)malloc(length);
    if (copy != NULL) {
        memcpy(copy, source, length);
    }

    return copy;
}


static void freeRequestFields(StudentRequest *request) {
    if (request == NULL) {
        return;
    }

    free(request->name);
    free(request->service_type);
    request->name = NULL;
    request->service_type = NULL;
}


static void destroyQueue(StudentRequestQueue *queue) {
    if (queue == NULL) {
        return;
    }

    QueueNode *current = queue->front;
    while (current != NULL) {
        QueueNode *next = current->next;
        freeRequestFields(&current->request);
        free(current);
        current = next;
    }

    queue->front = NULL;
    queue->rear = NULL;
    queue->count = 0;
}


static void destroyStack(StudentRequestStack *stack) {
    if (stack == NULL) {
        return;
    }

    StackNode *current = stack->top;
    while (current != NULL) {
        StackNode *next = current->next;
        freeRequestFields(&current->request);
        free(current);
        current = next;
    }

    stack->top = NULL;
    stack->count = 0;
}


static void destroyList(CompletedServiceList *list) {
    if (list == NULL) {
        return;
    }

    ListNode *current = list->head;
    while (current != NULL) {
        ListNode *next = current->next;
        freeRequestFields(&current->request);
        free(current);
        current = next;
    }

    list->head = NULL;
    list->count = 0;
}


Response getMenuChoice(int *choice) {
    char buffer[128];
    printf("Enter your choice (1-8): ");

    // Read a line of input and validate it
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid input.");
    }

    // Validate that the input is an integer and within the valid range
    int len = 0;
    if (sscanf(buffer, "%d%n", choice, &len) != 1) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid choice.");
    }

    // Check for extra characters after the number
    if (buffer[len] != '\n' && buffer[len] != '\r' && buffer[len] != '\0' && buffer[len] != ' ') {
        return makeResponse(ERROR_INVALID_INPUT, "Extra characters after choice.");
    }

    // Validate the choice range
    if (*choice < 1 || *choice > 8) {
        return makeResponse(ERROR_INVALID_INPUT, "Choice must be between 1 and 8.");
    }

    return makeResponse(SUCCESS, "Menu choice obtained successfully.");
}


Response addNewStudentRequest(StudentRequestQueue *queue, StudentRequest *request) {
    if (queue == NULL || request == NULL || request->name == NULL || request->service_type == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    StudentRequest stored_request = *request;
    stored_request.name = duplicateString(request->name);
    stored_request.service_type = duplicateString(request->service_type);

    if (stored_request.name == NULL || stored_request.service_type == NULL) {
        freeRequestFields(&stored_request);
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to allocate memory for request details.");
    }

    stored_request.status = WAITING;

    Response response = enqueue(queue, &stored_request);
    if (response.code != SUCCESS) {
        freeRequestFields(&stored_request);
        return response;
    }

    return makeResponse(SUCCESS, "Student request added to the waiting queue successfully.");
}


Response serveNextStudent(StudentRequestQueue *queue, StudentRequestStack *stack) {
    if (queue == NULL || stack == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    StudentRequest next_request;
    Response response = dequeue(queue, &next_request);
    if (response.code != SUCCESS) {
        return response;
    }

    next_request.status = SERVICING;
    response = push(stack, &next_request);
    if (response.code != SUCCESS) {
        next_request.status = WAITING;
        Response rollback_response = enqueue(queue, &next_request);
        if (rollback_response.code != SUCCESS) {
            return rollback_response;
        }

        return response;
    }

    return makeResponse(SUCCESS, "Next student request served successfully.");
}


Response displayWaitingQueue(const StudentRequestQueue *queue) {
    return displayQueue(queue);
}


Response displayCompletedServices(const CompletedServiceList *list) {
    return displayList(list);
}


Response searchCompletedServiceByStudentID(const CompletedServiceList *list, int student_id, StudentRequest **result, size_t *count) {
    if (result == NULL || count == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    *result = NULL;
    *count = 0;
    return searchByStudentID(list, student_id, result, count);
}


Response undoLastCompletedService(StudentRequestStack *stack, StudentRequestQueue *queue, CompletedServiceList *list) {
    if (stack == NULL || queue == NULL || list == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    StudentRequest request;
    Response response = pop(stack, &request);
    if (response.code != SUCCESS) {
        return response;
    }

    request.status = WAITING;
    response = enqueue(queue, &request);
    if (response.code != SUCCESS) {
        request.status = SERVICING;
        Response rollback_response = push(stack, &request);
        if (rollback_response.code != SUCCESS) {
            return rollback_response;
        }

        return response;
    }

    response = removeLatestCompletedService(list);
    if (response.code != SUCCESS) {
        request.status = SERVICING;
        Response rollback_response = push(stack, &request);
        if (rollback_response.code != SUCCESS) {
            return rollback_response;
        }
        request.status = WAITING;
        Response rollback_enqueue_response = enqueue(queue, &request);
        if (rollback_enqueue_response.code != SUCCESS) {
            return rollback_enqueue_response;
        }
        return response;
    }
    
    printf("Undo successful!\n");
    printf("The following student has been returned to the waiting queue:\n");
    printf("%d  %-*s  %-*s",
        request.student_id,
        (int)strlen(request.name), request.name,
        (int)strlen(request.service_type), request.service_type
    );

    return makeResponse(SUCCESS, "Last completed service undone successfully.");
}


Response displaySystemSummary(const StudentRequestQueue *queue, const StudentRequestStack *stack, const CompletedServiceList *list) {
    if (queue == NULL || stack == NULL || list == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    size_t completed_count = 0;
    Response response = CountCompletedServices(list, &completed_count);
    if (response.code != SUCCESS) {
        return response;
    }

    printf("\nSystem Summary:\n");
    printf("  Waiting requests : %zu\n", queue->count);
    printf("  Serving requests : %zu\n", stack->count);
    printf("  Completed records : %zu\n", completed_count);

    return makeResponse(SUCCESS, "System summary displayed successfully.");
}


Response shutdownSystem(StudentRequestQueue *queue, StudentRequestStack *stack, CompletedServiceList *list) {
    if (queue == NULL || stack == NULL || list == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    destroyQueue(queue);
    destroyStack(stack);
    destroyList(list);

    return makeResponse(SUCCESS, "System state cleaned up successfully.");
}


