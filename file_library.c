#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "file_library.h"

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
    printf("Class data saved successfully.\n");
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
        return NULL;
    }

    if (getcwd(buffer, MAX_PATH) != NULL) {
        strcat(buffer, "/");
        strcat(buffer, filename);
        return buffer;
    }

    free(buffer);
    return NULL;
}
