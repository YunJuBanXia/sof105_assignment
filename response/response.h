// response.h
#ifndef RESPONSE_H
#define RESPONSE_H


typedef enum {
    SUCCESS = 0,

    ERROR_INVALID_INPUT,
    ERROR_QUEUE_EMPTY,
    ERROR_STACK_EMPTY,
    ERROR_STACK_FULL,
    ERROR_NOT_FOUND,

    ERROR_MEMORY_ALLOCATION,
    ERROR_INVALID_PARAMETER,
    ERROR_INTERNAL,  // Other internal errors
} StatusCode;


typedef struct {
    StatusCode code;
    const char* message;
} Response;


Response makeResponse(const StatusCode code, const char* message);

void printError(Response error);


#endif // RESPONSE_H