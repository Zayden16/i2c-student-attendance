#include <stdio.h>
#include <string.h>
#include "student_library.h"
#include "auth_library.h"
#include "class_library.h"

// Function declarations
void displayLoginMenu(void);
void displayMainMenu(void);
void displayClassMenu(Class* class);
void handleLogin(void);
void handleRegistration(void);
void handleClassOperations(SchoolSystem* school, Class* class);

// Function definitions
void displayLoginMenu(void) {
    printf("\nStudent Attendance Record System - Login\n");
    printf("1. Login\n");
    printf("2. Register (Admin approval required)\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

void displayMainMenu(void) {
    Teacher* current = getCurrentTeacher();
    printf("\nWelcome, %s!\n", current->username);
    printf("\nStudent Attendance Record System\n");
    printf("1. List Classes\n");
    printf("2. Create New Class%s\n", current->is_admin ? "" : " (Admin only)");
    printf("3. Select Class\n");
    printf("4. Logout\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

void displayClassMenu(Class* class) {
    printf("\nClass: %s (ID: %d)\n", class->className, class->classId);
    printf("1. Take Attendance\n");
    printf("2. Display Attendance\n");
    printf("3. Generate Report\n");
    printf("4. Add Student\n");
    printf("5. Save Class Data\n");
    printf("6. Return to Main Menu\n");
    printf("Enter your choice: ");
}

void handleLogin(void) {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter username: ");
    scanf(" %[^\n]", username);
    printf("Enter password: ");
    scanf(" %[^\n]", password);

    if (loginTeacher(username, password, NULL)) {
        printf("Login successful!\n");
    } else {
        printf("Invalid username or password.\n");
    }
}

void handleRegistration(void) {
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter desired username: ");
    scanf(" %[^\n]", username);
    printf("Enter password (minimum 8 characters, must include uppercase, lowercase, and number): ");
    scanf(" %[^\n]", password);

    if (registerTeacher(username, password, false)) {
        printf("Registration successful! You can now login.\n");
    } else {
        printf("Registration failed. Username may be taken or password requirements not met.\n");
    }
}

void handleClassOperations(SchoolSystem* school, Class* class) {
    int choice;
    do {
        displayClassMenu(class);
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                takeAttendance(class);
                break;
            case 2:
                displayAttendance(class);
                break;
            case 3:
                generateReport(class);
                break;
            case 4: {
                char name[MAX_NAME_LENGTH];
                printf("Enter student name: ");
                scanf(" %[^\n]", name);
                addStudent(class, name);
                break;
            }
            case 5:
                saveSchoolSystem(school);
                printf("Class data saved successfully.\n");
                break;
            case 6:
                printf("Returning to main menu.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);
}

int main() {
    if (!initializeAuthSystem()) {
        printf("Failed to initialize authentication system.\n");
        return 1;
    }

    SchoolSystem school;
    loadSchoolSystem(&school);

    int choice;
    do {
        if (!isLoggedIn()) {
            displayLoginMenu();
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    handleLogin();
                    break;
                case 2:
                    handleRegistration();
                    break;
                case 3:
                    printf("Exiting program.\n");
                    break;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        } else {
            Teacher* current = getCurrentTeacher();
            displayMainMenu();
            scanf("%d", &choice);

            switch (choice) {
                case 1: {  // List all classes
                    if (current->is_admin) {
                        listAllClasses(&school);
                    } else {
                        listTeacherClasses(&school, current->teacher_id);
                    }
                    break;
                }
                case 2: {  // Create new class
                    if (current->is_admin) {
                        char className[MAX_NAME_LENGTH];
                        int teacherId;
                        printf("Enter class name: ");
                        scanf(" %[^\n]", className);
                        printf("Enter teacher ID: ");
                        scanf("%d", &teacherId);
                        if (createClass(&school, className, teacherId)) {
                            printf("Class created successfully.\n");
                        }
                    } else {
                        printf("Only administrators can create classes.\n");
                    }
                    break;
                }
                case 3: {  // Select and manage class
                    int classId;
                    printf("Enter class ID: ");
                    scanf("%d", &classId);

                    Class* selectedClass = getClassById(&school, classId);
                    if (selectedClass == NULL) {
                        printf("Class not found.\n");
                        break;
                    }

                    if (!current->is_admin && !isTeacherAssignedToClass(selectedClass, current->teacher_id)) {
                        printf("You don't have access to this class.\n");
                        break;
                    }

                    handleClassOperations(&school, selectedClass);
                    break;
                }
                case 4:  // Logout
                    logout();
                    printf("Logged out successfully.\n");
                    break;
                case 5:  // Exit
                    printf("Exiting program.\n");
                    break;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        }
    } while (choice != 3 && (!isLoggedIn() || choice != 5));

    saveSchoolSystem(&school);
    return 0;
}