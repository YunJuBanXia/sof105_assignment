// stack/stack.c
#include "stack.h"

#include <stddef.h>
#include <stdlib.h>


Response isEmpty(const StudentRequestStack *stack, int *result) {
    if (stack == NULL || result == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    *result = (stack->count == 0) ? 1 : 0;
    return makeResponse(SUCCESS, "Checked if stack is empty successfully.");
}


Response initStack(StudentRequestStack *stack) {
    if (stack == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    stack->top = NULL;
    stack->count = 0;
    return makeResponse(SUCCESS, "Stack initialized successfully.");
}


Response push(StudentRequestStack *stack, StudentRequest *request) {
    if (stack == NULL || request == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    StackNode* new_node = (StackNode*)malloc(sizeof(StackNode));

    if (new_node == NULL) {
        return makeResponse(ERROR_MEMORY_ALLOCATION, "Failed to allocate memory for new request.");
    }
    new_node->request = *request;  // Copy the request data
    new_node->next = stack->top; // Insert at the top of the stack

    stack->top = new_node;
    stack->count++;

    return makeResponse(SUCCESS, "Request pushed onto stack successfully.");
}


Response pop(StudentRequestStack *stack, StudentRequest *request) {  // The request parameter is used to return the removed request
    if (stack == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    if (stack->count == 0) {
        return makeResponse(ERROR_STACK_EMPTY, "Stack is empty.");
    }

    StackNode* temp = stack->top;
    *request = temp->request;  // Copy the request data

    stack->top = stack->top->next;
    free(temp);
    stack->count--;

    return makeResponse(SUCCESS, "Request popped from stack successfully.");
}


Response peek(const StudentRequestStack *stack, StudentRequest *request) {  // The request parameter is used to return the top request
    if (stack == NULL) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    if (stack->count == 0) {
        return makeResponse(ERROR_STACK_EMPTY, "Stack is empty.");
    }

    *request = stack->top->request;  // Copy the request data

    return makeResponse(SUCCESS, "Request peeked successfully.");
}
