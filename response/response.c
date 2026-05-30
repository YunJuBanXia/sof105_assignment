// errors.c
#include "errors.h"

#include <stdio.h>


Response makeResponse(const StatusCode code, const char* message) {
    if (message == NULL) {
        message = "No additional information provided.";
    }

    return (Response) {
        .code = code,
        .message = message
    };
}


void printError(Response error, const char* context) {
    // Only print if it's an error
    if (error.code == SUCCESS) {
        return;
    }

    // Print the error message in a formatted way
    printf("\n\n==================================\n\n");
    printf("Error Occurred!\n");
    printf("Message: %s\n", error.message);
    if (context != NULL) {
        printf("Context: %s\n", context);
    }
    printf("\n\n==================================\n\n");
}