#ifndef AUTH_LIBRARY_H
#define AUTH_LIBRARY_H

#include <stdbool.h>

#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_TEACHERS 20
#define TEACHER_FILE "teachers.txt"

typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];  // Will store hashed password
    bool is_admin;
    int teacher_id;
} Teacher;

// Authentication functions
bool initializeAuthSystem(void);
bool registerTeacher(const char* username, const char* password, bool is_admin);
bool loginTeacher(const char* username, const char* password, Teacher* logged_teacher);
bool changePassword(const char* username, const char* old_password, const char* new_password);
bool saveTeacherData(void);
bool loadTeacherData(void);

// Password utilities
void hashPassword(const char* password, char* hashed);
bool validatePassword(const char* password);

// Session management
bool isLoggedIn(void);
void logout(void);
Teacher* getCurrentTeacher(void);

#endif // AUTH_LIBRARY_H