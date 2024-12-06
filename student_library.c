#include <stdio.h>
#include <string.h>
#include "student_library.h"

void initializeClass(Class *class, int classId, const char *className) {
    class->classId = classId;
    strncpy(class->className, className, MAX_NAME_LENGTH - 1);
    class->className[MAX_NAME_LENGTH - 1] = '\0';
    class->studentCount = 0;
    class->nextStudentId = 1;
}

void addStudent(Class *class, const char *name) {
    if (class->studentCount < MAX_STUDENTS) {
        Student *newStudent = &class->students[class->studentCount];
        newStudent->id = class->nextStudentId++;
        strncpy(newStudent->name, name, MAX_NAME_LENGTH - 1);
        newStudent->name[MAX_NAME_LENGTH - 1] = '\0';
        newStudent->present = false;
        class->studentCount++;
        printf("Student added successfully with ID: %d\n", newStudent->id);
    } else {
        printf("Class is full. Cannot add more students.\n");
    }
}

void takeAttendance(Class *class) {
    printf("Taking attendance for %s\n", class->className);
    for (int i = 0; i < class->studentCount; i++) {
        char response;
        printf("Is %s (ID: %d) present? (y/n): ", class->students[i].name, class->students[i].id);
        scanf(" %c", &response);
        class->students[i].present = (response == 'y' || response == 'Y');
    }
    printf("Attendance recorded successfully.\n");
}

void displayAttendance(Class *class) {
    printf("Attendance for %s:\n", class->className);
    printf("ID\tName\t\tStatus\n");
    for (int i = 0; i < class->studentCount; i++) {
        printf("%d\t%-20s%s\n", class->students[i].id, class->students[i].name,
               class->students[i].present ? "Present" : "Absent");
    }
}

void generateReport(Class *class) {
    int presentCount = 0;
    for (int i = 0; i < class->studentCount; i++) {
        if (class->students[i].present) {
            presentCount++;
        }
    }
    float attendanceRate = (float)presentCount / class->studentCount * 100;

    printf("Attendance Report for %s:\n", class->className);
    printf("Total Students: %d\n", class->studentCount);
    printf("Present: %d\n", presentCount);
    printf("Absent: %d\n", class->studentCount - presentCount);
    printf("Attendance Rate: %.2f%%\n", attendanceRate);
}
