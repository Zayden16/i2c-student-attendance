#include <stdio.h>
#include "student_library.h"
#include "file_library.h"

int main() {
    Class myClass;
    loadClassFromFile(&myClass);

    int choice;
    do {
        printf("\nStudent Attendance Record System\n");
        printf("1. Take Attendance\n");
        printf("2. Display Attendance\n");
        printf("3. Generate Report\n");
        printf("4. Add Student\n");
        printf("5. Save Class Data\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                takeAttendance(&myClass);
            break;
            case 2:
                displayAttendance(&myClass);
            break;
            case 3:
                generateReport(&myClass);
            break;
            case 4: {
                char name[MAX_NAME_LENGTH];
                printf("Enter student name: ");
                scanf(" %[^\n]", name);
                addStudent(&myClass, name);
                break;
            }
            case 5:
                saveClassToFile(&myClass);
            break;
            case 6:
                printf("Exiting program.\n");
            break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);

    return 0;
}
