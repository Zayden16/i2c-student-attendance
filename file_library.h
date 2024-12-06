#ifndef FILE_LIBRARY_H
#define FILE_LIBRARY_H

#include "student_library.h"

#define FILENAME "class_data.txt"
#define MAX_PATH 1024

void saveClassToFile(Class *class);
void loadClassFromFile(Class *class);
char* getFullPath(const char* filename);

#endif
