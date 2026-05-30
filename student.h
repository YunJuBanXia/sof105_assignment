// student.h
#ifndef STUDENT_H
#define STUDENT_H


typedef enum {
    NORMAL = 0,
    URGENT = 1
} PriorityLevel;


typedef enum {
    WAITING,
    SERVICING,
    COMPLETED
} ServiceStatus;


typedef struct {
    int student_id;
    char* name;
    char* service_type;
    PriorityLevel priority;
    ServiceStatus status;
} StudentRequest;


#endif // STUDENT_H