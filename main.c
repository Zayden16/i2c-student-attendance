#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

#define MAX_STUDENTS 100
#define MAX_NAME_LENGTH 50
#define FILENAME "class_data.txt"
#define MAX_PATH 1024

typedef struct { //test test
    int id;
    char name[MAX_NAME_LENGTH];
    bool present;
} Student;

typedef struct {
    int classId;
    char className[MAX_NAME_LENGTH];
    Student students[MAX_STUDENTS];
    int studentCount;
    int nextStudentId;
} Class;

// Function prototypes
void initializeClass(Class *class, int classId, const char *className);
void addStudent(Class *class, const char *name);
void takeAttendance(Class *class);
void displayAttendance(Class *class);
void generateReport(Class *class);
void saveClassToFile(Class *class);
void loadClassFromFile(Class *class);
char* getFullPath(const char* filename);

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

    time_t t = time(nullptr);
    struct tm *tm = localtime(&t);
    char date[20];
    strftime(date, sizeof(date), "%Y-%m-%d", tm);

    printf("Attendance Report for %s (Date: %s):\n", class->className, date);
    printf("Total Students: %d\n", class->studentCount);
    printf("Present: %d\n", presentCount);
    printf("Absent: %d\n", class->studentCount - presentCount);
    printf("Attendance Rate: %.2f%%\n", attendanceRate);
}

void saveClassToFile(Class *class) {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    fprintf(file, "%d\n%s\n", class->classId, class->className);
    fprintf(file, "%d\n", class->studentCount);
    fprintf(file, "%d\n", class->nextStudentId);

    for (int i = 0; i < class->studentCount; i++) {
        fprintf(file, "%d,%s\n", class->students[i].id, class->students[i].name);
    }

    fclose(file);

    char *fullPath = getFullPath(FILENAME);
    if (fullPath) {
        printf("Class data saved to file: %s\n", fullPath);
        free(fullPath);
    } else {
        printf("Class data saved, but unable to determine full path.\n");
    }
}

void loadClassFromFile(Class *class) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("No existing class data found. Starting with an empty class.\n");
        initializeClass(class, 1, "New Class");
        return;
    }

    fscanf(file, "%d\n", &class->classId);
    fgets(class->className, MAX_NAME_LENGTH, file);
    class->className[strcspn(class->className, "\n")] = 0; // Remove newline

    fscanf(file, "%d\n", &class->studentCount);
    fscanf(file, "%d\n", &class->nextStudentId);

    for (int i = 0; i < class->studentCount; i++) {
        fscanf(file, "%d,%[^\n]\n", &class->students[i].id, class->students[i].name);
        class->students[i].present = false;
    }

    fclose(file);
    printf("Class data loaded successfully.\n");
}

char* getFullPath(const char* filename) {
    char* buffer = malloc(MAX_PATH * sizeof(char));
    if (buffer == NULL) {
        return nullptr;
    }

    if (getcwd(buffer, MAX_PATH) != NULL) {
        size_t path_len = strlen(buffer);
        size_t filename_len = strlen(filename);

        if (path_len + filename_len + 2 > MAX_PATH) {
            free(buffer);
            return nullptr;
        }

        buffer[path_len] = '/';
        strcpy(buffer + path_len + 1, filename);
        return buffer;
    }
    free(buffer);
    return nullptr;
}
