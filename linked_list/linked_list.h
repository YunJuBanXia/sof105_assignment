// linked_list/linked_list.h
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "../response/response.h"
#include "../student.h"


typedef struct ListNode {
    StudentRequest request;
    struct ListNode* next;
} ListNode;


typedef struct {
    ListNode *head;
    size_t count;
} CompletedServiceList;


size_t CountCompletedServices(const CompletedServiceList *list);

Response initList(CompletedServiceList *list);
Response addToList(CompletedServiceList *list, StudentRequest *request);
Response displayList(const CompletedServiceList *list);
Response searchByStudentID(const CompletedServiceList *list, int student_id, StudentRequest **result, size_t *count);  // result is used to return an array of matching requests


#endif // LINKED_LIST_H