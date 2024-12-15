#include <stdio.h>
#include <string.h>
#include "class_library.h"

void initializeSchoolSystem(SchoolSystem* school) {
    school->classCount = 0;
    school->nextClassId = 1;
}

bool createClass(SchoolSystem* school, const char* className, int teacherId) {
    if (school->classCount >= MAX_CLASSES) {
        printf("Maximum number of classes reached.\n");
        return false;
    }

    Class* newClass = &school->classes[school->classCount];
    initializeClass(newClass, school->nextClassId, className);
    newClass->teacherId = teacherId;  // Add this field to Class struct
    
    school->classCount++;
    school->nextClassId++;
    
    return saveSchoolSystem(school);
}

bool deleteClass(SchoolSystem* school, int classId) {
    for (int i = 0; i < school->classCount; i++) {
        if (school->classes[i].classId == classId) {
            // Move all classes after this one back one position
            for (int j = i; j < school->classCount - 1; j++) {
                school->classes[j] = school->classes[j + 1];
            }
            school->classCount--;
            return saveSchoolSystem(school);
        }
    }
    return false;
}

Class* getClassById(SchoolSystem* school, int classId) {
    for (int i = 0; i < school->classCount; i++) {
        if (school->classes[i].classId == classId) {
            return &school->classes[i];
        }
    }
    return NULL;
}

Class* getClassesByTeacher(SchoolSystem* school, int teacherId, int* count) {
    static Class teacherClasses[MAX_CLASSES];
    *count = 0;
    
    for (int i = 0; i < school->classCount; i++) {
        if (school->classes[i].teacherId == teacherId) {
            teacherClasses[*count] = school->classes[i];
            (*count)++;
        }
    }
    
    return *count > 0 ? teacherClasses : NULL;
}

bool saveSchoolSystem(SchoolSystem* school) {
    FILE* file = fopen(CLASSES_FILE, "w");
    if (file == NULL) {
        printf("Error opening classes file for writing.\n");
        return false;
    }

    fprintf(file, "%d\n%d\n", school->classCount, school->nextClassId);

    for (int i = 0; i < school->classCount; i++) {
        Class* class = &school->classes[i];
        fprintf(file, "%d,%s,%d\n", class->classId, class->className, class->teacherId);
        
        // Save students for this class
        fprintf(file, "%d\n", class->studentCount);
        for (int j = 0; j < class->studentCount; j++) {
            Student* student = &class->students[j];
            fprintf(file, "%d,%s\n", student->id, student->name);
        }
    }

    fclose(file);
    return true;
}

bool loadSchoolSystem(SchoolSystem* school) {
    FILE* file = fopen(CLASSES_FILE, "r");
    if (file == NULL) {
        printf("No existing school data found. Starting with empty system.\n");
        initializeSchoolSystem(school);
        return true;
    }

    if (fscanf(file, "%d\n%d\n", &school->classCount, &school->nextClassId) != 2) {
        fclose(file);
        return false;
    }

    for (int i = 0; i < school->classCount; i++) {
        Class* class = &school->classes[i];
        
        // Read class information
        if (fscanf(file, "%d,%[^,],%d\n", 
                   &class->classId, 
                   class->className,
                   &class->teacherId) != 3) {
            fclose(file);
            return false;
        }

        // Read students for this class
        if (fscanf(file, "%d\n", &class->studentCount) != 1) {
            fclose(file);
            return false;
        }

        for (int j = 0; j < class->studentCount; j++) {
            Student* student = &class->students[j];
            if (fscanf(file, "%d,%[^\n]\n", 
                      &student->id, 
                      student->name) != 2) {
                fclose(file);
                return false;
            }
            student->present = false;
        }
    }

    fclose(file);
    return true;
}

void listAllClasses(SchoolSystem* school) {
    printf("\nAll Classes:\n");
    printf("ID\tClass Name\tTeacher ID\tStudent Count\n");
    printf("------------------------------------------------\n");
    
    for (int i = 0; i < school->classCount; i++) {
        Class* class = &school->classes[i];
        printf("%d\t%-16s%d\t\t%d\n", 
               class->classId, 
               class->className, 
               class->teacherId, 
               class->studentCount);
    }
}

void listTeacherClasses(SchoolSystem* school, int teacherId) {
    printf("\nYour Classes:\n");
    printf("ID\tClass Name\tStudent Count\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < school->classCount; i++) {
        Class* class = &school->classes[i];
        if (class->teacherId == teacherId) {
            printf("%d\t%-16s%d\n", 
                   class->classId, 
                   class->className, 
                   class->studentCount);
        }
    }
}

bool isTeacherAssignedToClass(const Class* class, int teacherId) {
    return class->teacherId == teacherId;
}