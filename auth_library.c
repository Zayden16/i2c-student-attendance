#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "auth_library.h"

// Global variables for authentication state
static Teacher teachers[MAX_TEACHERS];
static int teacher_count = 0;
static Teacher* current_teacher = NULL;
static bool system_initialized = false;

// Simple XOR-based hashing for demonstration (should use a proper hashing algorithm in production)
void hashPassword(const char* password, char* hashed) {
    const char salt[] = "SaltKey123";  // In production, use a random salt per user
    int i;
    for(i = 0; password[i] != '\0'; i++) {
        hashed[i] = password[i] ^ salt[i % (sizeof(salt) - 1)];
    }
    hashed[i] = '\0';
}

bool validatePassword(const char* password) {
    // Password must be at least 8 characters long
    if (strlen(password) < 8) return false;
    
    bool has_upper = false, has_lower = false, has_digit = false;
    
    for(int i = 0; password[i] != '\0'; i++) {
        if(password[i] >= 'A' && password[i] <= 'Z') has_upper = true;
        if(password[i] >= 'a' && password[i] <= 'z') has_lower = true;
        if(password[i] >= '0' && password[i] <= '9') has_digit = true;
    }
    
    return has_upper && has_lower && has_digit;
}

bool initializeAuthSystem(void) {
    if (system_initialized) return true;
    
    // Try to load existing teacher data
    if (!loadTeacherData()) {
        // If no existing data, create default admin account
        if (!registerTeacher("admin", "Admin123", true)) {
            return false;
        }
    }
    
    system_initialized = true;
    return true;
}

bool registerTeacher(const char* username, const char* password, bool is_admin) {
    if (teacher_count >= MAX_TEACHERS) return false;
    
    // Check if username already exists
    for(int i = 0; i < teacher_count; i++) {
        if (strcmp(teachers[i].username, username) == 0) return false;
    }
    
    // Validate password
    if (!validatePassword(password)) return false;
    
    // Create new teacher account
    Teacher* new_teacher = &teachers[teacher_count];
    strncpy(new_teacher->username, username, MAX_USERNAME_LENGTH - 1);
    new_teacher->username[MAX_USERNAME_LENGTH - 1] = '\0';
    
    char hashed_password[MAX_PASSWORD_LENGTH];
    hashPassword(password, hashed_password);
    strncpy(new_teacher->password, hashed_password, MAX_PASSWORD_LENGTH - 1);
    new_teacher->password[MAX_PASSWORD_LENGTH - 1] = '\0';
    
    new_teacher->is_admin = is_admin;
    new_teacher->teacher_id = teacher_count + 1;
    
    teacher_count++;
    
    return saveTeacherData();
}

bool loginTeacher(const char* username, const char* password, Teacher* logged_teacher) {
    char hashed_password[MAX_PASSWORD_LENGTH];
    hashPassword(password, hashed_password);
    
    for(int i = 0; i < teacher_count; i++) {
        if (strcmp(teachers[i].username, username) == 0 &&
            strcmp(teachers[i].password, hashed_password) == 0) {
            current_teacher = &teachers[i];
            if (logged_teacher != NULL) {
                *logged_teacher = teachers[i];
            }
            return true;
        }
    }
    
    return false;
}

bool changePassword(const char* username, const char* old_password, const char* new_password) {
    // Find teacher
    Teacher* teacher = NULL;
    for(int i = 0; i < teacher_count; i++) {
        if (strcmp(teachers[i].username, username) == 0) {
            teacher = &teachers[i];
            break;
        }
    }
    
    if (teacher == NULL) return false;
    
    // Verify old password
    char hashed_old[MAX_PASSWORD_LENGTH];
    hashPassword(old_password, hashed_old);
    if (strcmp(teacher->password, hashed_old) != 0) return false;
    
    // Validate and set new password
    if (!validatePassword(new_password)) return false;
    
    char hashed_new[MAX_PASSWORD_LENGTH];
    hashPassword(new_password, hashed_new);
    strncpy(teacher->password, hashed_new, MAX_PASSWORD_LENGTH - 1);
    teacher->password[MAX_PASSWORD_LENGTH - 1] = '\0';
    
    return saveTeacherData();
}

bool saveTeacherData(void) {
    FILE* file = fopen(TEACHER_FILE, "w");
    if (file == NULL) return false;
    
    fprintf(file, "%d\n", teacher_count);
    for(int i = 0; i < teacher_count; i++) {
        fprintf(file, "%s,%s,%d,%d\n",
                teachers[i].username,
                teachers[i].password,
                teachers[i].is_admin,
                teachers[i].teacher_id);
    }
    
    fclose(file);
    return true;
}

bool loadTeacherData(void) {
    FILE* file = fopen(TEACHER_FILE, "r");
    if (file == NULL) return false;
    
    if (fscanf(file, "%d\n", &teacher_count) != 1) {
        fclose(file);
        return false;
    }
    
    for(int i = 0; i < teacher_count; i++) {
        Teacher* t = &teachers[i];
        if (fscanf(file, "%[^,],%[^,],%d,%d\n",
                   t->username,
                   t->password,
                   &t->is_admin,
                   &t->teacher_id) != 4) {
            fclose(file);
            return false;
        }
    }
    
    fclose(file);
    return true;
}

bool isLoggedIn(void) {
    return current_teacher != NULL;
}

void logout(void) {
    current_teacher = NULL;
}

Teacher* getCurrentTeacher(void) {
    return current_teacher;
}