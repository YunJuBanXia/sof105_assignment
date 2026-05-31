// services/services.c
#include "services.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static char *duplicateString(const char *source) {
    // This function is used to create a duplicate of a string, which is necessary when adding requests to the completed service list
    // There is a same function in linked_list/linked_list.c, but we need it here as well to avoid circular dependency between services and linked_list modules
    // The static keyword limits the scope of this function to this file
    // So it won't cause any conflict with the duplicateString function in linked_list/linked_list.c
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
    // This function is used to free the dynamically allocated fields of a StudentRequest
    // There is a same function in linked_list/linked_list.c, but we need it here as well to avoid circular dependency between services and linked_list modules
    // The static keyword limits the scope of this function to this file
    // So it won't cause any conflict with the freeRequestFields function in linked_list/linked_list.c
    if (request == NULL) {
        return;
    }

    free(request->name);
    free(request->service_type);
    request->name = NULL;
    request->service_type = NULL;
}


static void destroyQueue(StudentRequestQueue *queue) {
    // This function is used to free all the nodes in the queue and reset it to an empty state
    // It is called during system shutdown to clean up resources
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
    // This function is used to free all the nodes in the stack and reset it to an empty state
    // It is called during system shutdown to clean up resources
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
    // This function is used to free all the nodes in the completed service list and reset it to an empty state
    // It is called during system shutdown to clean up resources
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
    // Corresponds to menu option 1: Add new student request
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

    // New requests are always added with WAITING status, and will be updated to SERVICING when they are served
    stored_request.status = WAITING;

    Response response = enqueue(queue, &stored_request);
    if (response.code != SUCCESS) {
        freeRequestFields(&stored_request);
        return response;
    }

    return makeResponse(SUCCESS, "Student request added to the waiting queue successfully.");
}


Response serveNextStudent(StudentRequestQueue *queue, StudentRequestStack *stack, CompletedServiceList *list) {
    // Corresponds to menu option 2: Serve next student
    if (queue == NULL || stack == NULL || list == NULL) {
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
        // backtrack to restore the queue state if pushing to stack fails
        next_request.status = WAITING;
        Response rollback_response = enqueue(queue, &next_request);
        if (rollback_response.code != SUCCESS) {
            return rollback_response;
        }
        return response;
    }

    response = addToList(list, &next_request);
    if (response.code != SUCCESS) {
        // backtrack to restore the stack and queue state if adding to list fails
        next_request.status = WAITING;
        Response rollback_stack_response = pop(stack, &next_request);
        if (rollback_stack_response.code != SUCCESS) {
            return rollback_stack_response;
        }
        Response rollback_queue_response = enqueue(queue, &next_request);
        if (rollback_queue_response.code != SUCCESS) {
            return rollback_queue_response;
        }
        return response;
    }

    return makeResponse(SUCCESS, "Next student request served successfully.");
}


Response displayWaitingQueue(const StudentRequestQueue *queue) {
    // Corresponds to menu option 3: Display waiting queue
    return displayQueue(queue);
}


Response displayCompletedServices(const CompletedServiceList *list) {
    // Corresponds to menu option 4: Display completed services
    return displayList(list);
}


Response searchCompletedServiceByStudentID(const CompletedServiceList *list, int student_id, StudentRequest **result, size_t *count) {
    // Corresponds to menu option 5: Search completed service by student ID
    return searchByStudentID(list, student_id, result, count);
}


Response undoLastCompletedService(StudentRequestStack *stack, StudentRequestQueue *queue, CompletedServiceList *list) {
    // Corresponds to menu option 6: Undo last completed service
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
        // backtrack to restore the stack state if enqueue fails
        request.status = SERVICING;
        Response rollback_response = push(stack, &request);
        if (rollback_response.code != SUCCESS) {
            return rollback_response;
        }

        return response;
    }

    response = removeLatestCompletedService(list);
    if (response.code != SUCCESS) {
        // backtrack to restore the queue and stack state if removing from list fails
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
    printf("%d  %-*s  %-*s",  // Format the output to align the columns based on the length of the name and service type
        request.student_id,
        (int)strlen(request.name), request.name,
        (int)strlen(request.service_type), request.service_type
    );

    return makeResponse(SUCCESS, "Last completed service undone successfully.");
}


Response displaySystemSummary(const StudentRequestQueue *queue, const StudentRequestStack *stack, const CompletedServiceList *list) {
    // Corresponds to menu option 7: Display system summary
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
    // Corresponds to menu option 8: Exit
    if (queue == NULL || stack == NULL || list == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    destroyQueue(queue);
    destroyStack(stack);
    destroyList(list);

    return makeResponse(SUCCESS, "System state cleaned up successfully.");
}


