// errors.h
#ifndef ERRORS_H
#define ERRORS_H


typedef enum {
    SUCCESS = 0,

    ERROR_INVALID_INPUT,
    ERROR_QUEUE_EMPTY,
    ERROR_STACK_EMPTY,
    ERROR_STACK_FULL,
    ERROR_NOT_FOUND,

    ERROR_MEMORY_ALLOCATION,
    ERROR_INTERNAL,  // Other internal errors
} StatusCode;


typedef struct {
    const StatusCode code;
    const char* message;
} Response;


Response makeResponse(const StatusCode code, const char* message);

void printError(Response error, const char* context);


#endif // ERRORS_H