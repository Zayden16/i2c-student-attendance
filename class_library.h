#ifndef CLASS_LIBRARY_H
#define CLASS_LIBRARY_H

#include "student_library.h"

#define MAX_CLASSES 50
#define CLASSES_FILE "classes.txt"

typedef struct {
    Class classes[MAX_CLASSES];
    int classCount;
    int nextClassId;
} SchoolSystem;

// Class management functions
void initializeSchoolSystem(SchoolSystem* school);
bool createClass(SchoolSystem* school, const char* className, int teacherId);
bool deleteClass(SchoolSystem* school, int classId);
Class* getClassById(SchoolSystem* school, int classId);
Class* getClassesByTeacher(SchoolSystem* school, int teacherId, int* count);

// Data persistence
bool saveSchoolSystem(SchoolSystem* school);
bool loadSchoolSystem(SchoolSystem* school);

// Class operations
void listAllClasses(SchoolSystem* school);
void listTeacherClasses(SchoolSystem* school, int teacherId);
bool isTeacherAssignedToClass(const Class* class, int teacherId);

#endif // CLASS_LIBRARY_H