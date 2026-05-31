// services/services.h
#ifndef SERVICES_H
#define SERVICES_H

#include "../linked_list/linked_list.h"
#include "../queue/queue.h"
#include "../response/response.h"
#include "../stack/stack.h"

Response getMenuChoice(int *choice);
Response addNewStudentRequest(StudentRequestQueue *queue, StudentRequest *request);
Response serveNextStudent(StudentRequestQueue *queue, StudentRequestStack *stack);
Response displayWaitingQueue(const StudentRequestQueue *queue);
Response displayCompletedServices(const CompletedServiceList *list);
Response searchCompletedServiceByStudentID(const CompletedServiceList *list, int student_id, StudentRequest **result, size_t *count);
Response undoLastCompletedService(StudentRequestStack *stack, StudentRequestQueue *queue, CompletedServiceList *list);
Response displaySystemSummary(const StudentRequestQueue *queue, const StudentRequestStack *stack, const CompletedServiceList *list);
Response shutdownSystem(StudentRequestQueue *queue, StudentRequestStack *stack, CompletedServiceList *list);    


#endif