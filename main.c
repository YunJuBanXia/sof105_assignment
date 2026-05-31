// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "services/services.h"


static void trimTrailingNewline(char *text) {
    if (text == NULL) {
        return;
    }

    size_t length = strlen(text);
    while (length > 0 && (text[length - 1] == '\n' || text[length - 1] == '\r')) {
        text[length - 1] = '\0';
        length--;
    }
}


static Response readLinePrompt(const char *prompt, char *buffer, size_t buffer_size) {
    if (prompt == NULL || buffer == NULL || buffer_size == 0) {
        return makeResponse(ERROR_INVALID_PARAMETER, "Invalid parameter provided.");
    }

    printf("%s", prompt);
    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        return makeResponse(ERROR_INVALID_INPUT, "Failed to read input.");
    }

    trimTrailingNewline(buffer);
    return makeResponse(SUCCESS, "Input read successfully.");
}


static Response readIntPrompt(const char *prompt, int *value) {
    char buffer[128];

    Response response = readLinePrompt(prompt, buffer, sizeof(buffer));
    if (response.code != SUCCESS) {
        return response;
    }

    char *end_pointer = NULL;
    long parsed_value = strtol(buffer, &end_pointer, 10);
    if (end_pointer == buffer || *end_pointer != '\0') {
        return makeResponse(ERROR_INVALID_INPUT, "Please enter a valid integer.");
    }

    *value = (int)parsed_value;
    return makeResponse(SUCCESS, "Integer read successfully.");
}


static void printMenu(void) {
    printf("\n==== Student Service Management System ====\n");
    printf("1. Add new student request\n");
    printf("2. Serve next student\n");
    printf("3. Display waiting queue\n");
    printf("4. Display completed services\n");
    printf("5. Search completed service by student ID\n");
    printf("6. Undo last completed service\n");
    printf("7. Display system summary\n");
    printf("8. Exit\n");
}


static void printSearchResults(const StudentRequest *results, size_t count) {
    if (results == NULL || count == 0) {
        return;
    }

    printf("\nSearch Results:\n");
    for (size_t index = 0; index < count; index++) {
        printf("\t%zu. %d  %s  %s  %s\n",
               index + 1,
               results[index].student_id,
               results[index].name,
               results[index].service_type,
               results[index].status == COMPLETED ? "Completed" :
               results[index].status == SERVICING ? "Servicing" : "Waiting");
    }
}


int main(void) {
    StudentRequestQueue queue;
    StudentRequestStack stack;
    CompletedServiceList list;

    Response queue_init_response = initQueue(&queue);
    if (queue_init_response.code != SUCCESS) {
        printError(queue_init_response);
        return EXIT_FAILURE;
    }

    Response stack_init_response = initStack(&stack);
    if (stack_init_response.code != SUCCESS) {
        printError(stack_init_response);
        return EXIT_FAILURE;
    }

    Response list_init_response = initList(&list);
    if (list_init_response.code != SUCCESS) {
        printError(list_init_response);
        return EXIT_FAILURE;
    }

    int running = 1;
    while (running) {
        int choice = 0;

        printMenu();
        Response choice_response = getMenuChoice(&choice);
        if (choice_response.code != SUCCESS) {
            printError(choice_response);
            continue;
        }

        switch (choice) {
            case 1: {
                StudentRequest request;
                char name_buffer[128];
                char service_buffer[128];
                int priority_choice = 0;

                Response student_id_response = readIntPrompt("Enter student ID: ", &request.student_id);
                if (student_id_response.code != SUCCESS) {
                    printError(student_id_response);
                    break;
                }

                Response name_response = readLinePrompt("Enter student name: ", name_buffer, sizeof(name_buffer));
                if (name_response.code != SUCCESS) {
                    printError(name_response);
                    break;
                }

                Response service_response = readLinePrompt("Enter service type: ", service_buffer, sizeof(service_buffer));
                if (service_response.code != SUCCESS) {
                    printError(service_response);
                    break;
                }

                Response priority_response = readIntPrompt("Enter priority (0 = Normal, 1 = Urgent): ", &priority_choice);
                if (priority_response.code != SUCCESS || (priority_choice != NORMAL && priority_choice != URGENT)) {
                    if (priority_response.code == SUCCESS) {
                        priority_response = makeResponse(ERROR_INVALID_INPUT, "Priority must be 0 or 1.");
                    }
                    printError(priority_response);
                    break;
                }

                request.name = name_buffer;
                request.service_type = service_buffer;
                request.priority = (PriorityLevel)priority_choice;
                request.status = WAITING;

                Response add_response = addNewStudentRequest(&queue, &request);
                if (add_response.code != SUCCESS) {
                    printError(add_response);
                    break;
                }

                printf("%s\n", add_response.message);
                break;
            }

            case 2: {
                Response serve_response = serveNextStudent(&queue, &stack);
                if (serve_response.code != SUCCESS) {
                    printError(serve_response);
                    break;
                }

                StudentRequest served_request;
                Response peek_response = peekStack(&stack, &served_request);
                if (peek_response.code != SUCCESS) {
                    printError(peek_response);
                    break;
                }

                Response list_response = addToList(&list, &served_request);
                if (list_response.code != SUCCESS) {
                    printError(list_response);
                    break;
                }

                printf("%s\n", serve_response.message);
                break;
            }

            case 3: {
                Response display_response = displayWaitingQueue(&queue);
                if (display_response.code != SUCCESS) {
                    printError(display_response);
                }
                break;
            }

            case 4: {
                Response display_response = displayCompletedServices(&list);
                if (display_response.code != SUCCESS) {
                    printError(display_response);
                }
                break;
            }

            case 5: {
                int student_id = 0;
                StudentRequest *results = NULL;
                size_t result_count = 0;

                Response id_response = readIntPrompt("Enter student ID to search: ", &student_id);
                if (id_response.code != SUCCESS) {
                    printError(id_response);
                    break;
                }

                Response search_response = searchCompletedServiceByStudentID(&list, student_id, &results, &result_count);
                if (search_response.code != SUCCESS) {
                    printError(search_response);
                    break;
                }

                printSearchResults(results, result_count);
                free(results);
                printf("%s\n", search_response.message);
                break;
            }

            case 6: {
                Response undo_response = undoLastCompletedService(&stack, &queue, &list);
                if (undo_response.code != SUCCESS) {
                    printError(undo_response);
                    break;
                }

                Response list_response = removeLatestCompletedService(&list);
                if (list_response.code != SUCCESS) {
                    printError(list_response);
                    break;
                }

                break;
            }

            case 7: {
                Response summary_response = displaySystemSummary(&queue, &stack, &list);
                if (summary_response.code != SUCCESS) {
                    printError(summary_response);
                }
                break;
            }

            case 8: {
                Response shutdown_response = shutdownSystem(&queue, &stack, &list);
                if (shutdown_response.code != SUCCESS) {
                    printError(shutdown_response);
                    break;
                }

                printf("%s\n", shutdown_response.message);
                running = 0;
                break;
            }

            default:
                printError(makeResponse(ERROR_INVALID_INPUT, "Unknown menu option."));
                break;
        }
    }

    return EXIT_SUCCESS;
}