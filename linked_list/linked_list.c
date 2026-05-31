// linked_list/linked_list.c
#include "linked_list.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static char *duplicateString(const char *source) {
    // This function is used to create a duplicate of a string, which is necessary when adding requests to the completed service list
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
    if (request == NULL) {
        return;
    }

    free(request->name);
    free(request->service_type);
    request->name = NULL;
    request->service_type = NULL;
}


Response CountCompletedServices(const CompletedServiceList *list, size_t *count) {
    if (list == NULL || count == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    *count = list->count;
    return makeResponse(SUCCESS, "Completed services counted successfully.");
}


Response initList(CompletedServiceList *list) {
    if (list == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    list->head = NULL;
    list->count = 0;
    return makeResponse(SUCCESS, "Completed service list initialized successfully.");
}


Response addToList(CompletedServiceList *list, StudentRequest *request) {
    if (list == NULL || request == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));

    if (new_node == NULL) {
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to allocate memory for new list node.");
    }
    new_node->request = *request;
    new_node->request.name = duplicateString(request->name);
    new_node->request.service_type = duplicateString(request->service_type);
    if (new_node->request.name == NULL || new_node->request.service_type == NULL) {
        freeRequestFields(&new_node->request);
        free(new_node);
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to copy completed request details.");
    }
    new_node->request.status = COMPLETED;
    new_node->next = list->head; // Insert at the beginning of the list

    list->head = new_node;
    list->count++;

    return makeResponse(SUCCESS, "Request added to completed service list successfully.");
}


Response removeLatestCompletedService(CompletedServiceList *list) {
    if (list == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    if (list->head == NULL) {
        return makeResponse(ERROR_QUEUE_EMPTY, "No completed services to remove.");
    }

    // Remove the head of the list, which is the latest completed service
    ListNode *temp = list->head;
    list->head = temp->next;
    list->count--;

    freeRequestFields(&temp->request);
    free(temp);

    return makeResponse(SUCCESS, "Latest completed service removed successfully.");
}


Response displayList(const CompletedServiceList *list) {
    if (list == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
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
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    *result = NULL;
    *count = 0;

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