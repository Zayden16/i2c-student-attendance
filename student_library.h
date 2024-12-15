#ifndef STUDENT_LIBRARY_H
#define STUDENT_LIBRARY_H

#include <stdbool.h>

#define MAX_STUDENTS 100
#define MAX_NAME_LENGTH 50

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    bool present;
} Student;

typedef struct {
    int classId;
    char className[MAX_NAME_LENGTH];
    int teacherId;
    Student students[MAX_STUDENTS];
    int studentCount;
    int nextStudentId;
} Class;

void initializeClass(Class *class, int classId, const char *className);
void addStudent(Class *class, const char *name);
void takeAttendance(Class *class);
void displayAttendance(Class *class);
void generateReport(Class *class);

#endif // STUDENT_LIBRARY_H