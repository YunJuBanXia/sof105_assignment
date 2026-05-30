// linked_list/linked_list.c
#include "linked_list.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>


size_t CountCompletedServices(const CompletedServiceList *list) {
    return list->count;
}


Response initList(CompletedServiceList *list) {
    if (list == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid input provided.");
    }

    list->head = NULL;
    list->count = 0;
    return makeResponse(SUCCESS, "Completed service list initialized successfully.");
}


Response addToList(CompletedServiceList *list, StudentRequest *request) {
    if (list == NULL || request == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid input provided.");
    }

    ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));

    if (new_node == NULL) {
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to allocate memory for new list node.");
    }
    new_node->request = *request;  // Copy the request data
    new_node->next = list->head; // Insert at the beginning of the list

    list->head = new_node;
    list->count++;

    return makeResponse(SUCCESS, "Request added to completed service list successfully.");
}


Response displayList(const CompletedServiceList *list) {
    if (list == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid input provided.");
    }

    if (list->head == NULL) {
        return makeResponse(ERROR_QUEUE_EMPTY, "No completed services to display.");
    }

    ListNode *current = list->head;

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

    // Second pass to print the list
    printf("\nCompleted Services:\n");
    current = list->head;
    int count = 1;
    while (current != NULL) {
        printf("\t%d. %d  %-*s  %-*s %s\n",
               count++,
               current->request.student_id,
               max_name_length, current->request.name,
               max_service_length, current->request.service_type,
               "Completed"
        );
        current = current->next;
    }

    return makeResponse(SUCCESS, "Completed services displayed successfully.");
}


Response searchByStudentID(const CompletedServiceList *list, int student_id, StudentRequest **result, size_t *count) {
    if (list == NULL || result == NULL || count == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Invalid input provided.");
    }

    ListNode *current = list->head;

    // First pass to count matches
    while (current != NULL) {
        if (current->request.student_id == student_id) {
            (*count)++;
        }
        current = current->next;
    }

    if (*count == 0) {
        // No result found
        *result = NULL;
        return makeResponse(ERROR_NOT_FOUND, "No completed services found for the given student ID.");
    }

    // Allocate memory for results
    *result = (StudentRequest*)malloc(*count * sizeof(StudentRequest));
    if (*result == NULL) {
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to allocate memory for search results.");
    }

    // Second pass to populate results
    current = list->head;
    size_t index = 0;
    while (current != NULL) {
        if (current->request.student_id == student_id) {
            (*result)[index++] = current->request; // Copy matching request to result array
        }
        current = current->next;
    }

    return makeResponse(SUCCESS, "Search completed successfully.");
}