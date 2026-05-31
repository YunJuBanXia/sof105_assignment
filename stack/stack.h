// stack/stack.h
#ifndef STACK_H
#define STACK_H

#include <stddef.h>

#include "../response/response.h"
#include "../student.h"


typedef struct StackNode {
    StudentRequest request;
    struct StackNode* next;
} StackNode;


typedef struct {
    StackNode *top;
    size_t count;
} StudentRequestStack;


Response isEmptyStack(const StudentRequestStack *stack, int *result);  // result is used to return whether the stack is empty (1 for empty, 0 for not empty)
Response initStack(StudentRequestStack *stack);
Response push(StudentRequestStack *stack, StudentRequest *request);
Response pop(StudentRequestStack *stack, StudentRequest *request);  // request parameter is used to return the removed request
Response peekStack(const StudentRequestStack *stack, StudentRequest *request);  // request parameter is used to return the top request without removing it


#endif // STACK_H