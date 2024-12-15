#include <stdio.h>
#include <string.h>

#include "attendance_analytics.h"
#include "student_library.h"
#include "auth_library.h"
#include "class_library.h"

void displayLoginMenu(void);
void displayMainMenu(void);
void displayClassMenu(Class* class);
void handleLogin(void);
void handleRegistration(void);
void handleClassOperations(SchoolSystem* school, Class* class);
void displayAnalyticsMenu(void);
void handleAnalytics(SchoolSystem* school, Class* class, AttendanceHistory* history);



void displayLoginMenu(void) {
    printf("\nStudent Attendance Record System - Login\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n");
    printf("If you do not have a user yet, please Register\n");
    printf("Enter your choice: ");
}


void displayMainMenu(void) {
    Teacher* current = getCurrentTeacher();
    printf("\nWelcome, %s!\n", current->username);
    printf("\nStudent Attendance Record System\n");
    printf("1. List Classes\n");
    printf("2. Create New Class\n");  // Removed admin-only note
    printf("3. Select Class\n");
    printf("4. Logout\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

void displayClassMenu(Class* class) {
    printf("\nClass: %s (ID: %d)\n", class->className, class->classId);
    printf("1. Take Attendance\n");
    printf("2. View Today's Attendance\n");
    printf("3. Add Student\n");
    printf("4. Analytics & Reports\n");
    printf("5. Save Class Data\n");
    printf("6. Return to Main Menu\n");
    printf("Enter your choice: ");
}

void displayAnalyticsMenu(void) {
    printf("\nAttendance Analytics & Reports\n");
    printf("-----------------------------\n");
    printf("1. Monthly Attendance Report\n");
    printf("2. Student Detailed Report\n");
    printf("3. Chronic Absentee Report\n");
    printf("4. Attendance Trends Analysis\n");
    printf("5. Return to Class Menu\n");
    printf("Enter your choice: ");
}


void handleAnalytics(SchoolSystem* school, Class* class, AttendanceHistory* history) {
    int choice;
    do {
        displayAnalyticsMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int month, year;
                printf("Enter month (1-12): ");
                scanf("%d", &month);
                printf("Enter year: ");
                scanf("%d", &year);
                generateMonthlyReport(history, class, month, year);
                break;
            }
            case 2: {
                int studentId;
                printf("Enter student ID: ");
                scanf("%d", &studentId);
                generateStudentDetailedReport(history, class, studentId);
                break;
            }
            case 3: {
                float threshold;
                printf("Enter attendance threshold percentage (e.g., 75.0): ");
                scanf("%f", &threshold);
                identifyChronicAbsentees(history, class, threshold);
                break;
            }
            case 4: {
                TrendAnalysis trends = analyzeTrends(history, class);
                printf("\nAttendance Trends Analysis\n");
                printf("==========================\n");

                printf("\nDaily Patterns:\n");
                const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                                    "Thursday", "Friday", "Saturday"};
                for (int i = 0; i < 7; i++) {
                    if (trends.weekdayRates[i] > 0) {
                        printf("%s: %.1f%%\n", days[i], trends.weekdayRates[i]);
                    }
                }

                printf("\nMonthly Patterns:\n");
                const char* months[] = {"January", "February", "March", "April",
                                      "May", "June", "July", "August",
                                      "September", "October", "November", "December"};
                for (int i = 0; i < 12; i++) {
                    if (trends.monthlyRates[i] > 0) {
                        printf("%s: %.1f%%\n", months[i], trends.monthlyRates[i]);
                    }
                }

                printf("\nOverall Trend: ");
                if (trends.overallTrend > 5) {
                    printf("Improving (↑%.1f%%)\n", trends.overallTrend);
                } else if (trends.overallTrend < -5) {
                    printf("Declining (↓%.1f%%)\n", -trends.overallTrend);
                } else {
                    printf("Stable\n");
                }
                break;
            }
            case 5:
                printf("Returning to class menu.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5);
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
    // Initialize or load attendance history
    char historyFilename[100];
    snprintf(historyFilename, sizeof(historyFilename), "attendance_history_%d.txt", class->classId);

    AttendanceHistory history;
    if (!loadAttendanceHistory(&history, historyFilename)) {
        initializeAttendanceHistory(&history, class->classId);
    }

    int choice;
    do {
        displayClassMenu(class);
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                printf("\nTaking attendance for %s\n", class->className);
                for (int i = 0; i < class->studentCount; i++) {
                    char response;
                    printf("Is %s (ID: %d) present? (y/n): ",
                           class->students[i].name,
                           class->students[i].id);
                    scanf(" %c", &response);
                    bool present = (response == 'y' || response == 'Y');
                    class->students[i].present = present;
                    addAttendanceRecord(&history, class->students[i].id, present);
                }
                saveAttendanceHistory(&history, historyFilename);
                printf("Attendance recorded successfully.\n");
                break;
            }
            case 2:
                printf("\nToday's Attendance:\n");
                printf("ID\tName\t\tStatus\n");
                printf("--------------------------------\n");
                for (int i = 0; i < class->studentCount; i++) {
                    printf("%d\t%-16s%s\n",
                           class->students[i].id,
                           class->students[i].name,
                           class->students[i].present ? "Present" : "Absent");
                }
                break;
            case 3: {
                char name[MAX_NAME_LENGTH];
                printf("Enter student name: ");
                scanf(" %[^\n]", name);
                addStudent(class, name);
                break;
            }
            case 4:
                handleAnalytics(school, class, &history);
                break;
            case 5: {
                saveSchoolSystem(school);
                saveAttendanceHistory(&history, historyFilename);
                printf("All data saved successfully.\n");
                break;
            }
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
                case 1: {
                    if (current->is_admin) {
                        listAllClasses(&school);
                    } else {
                        listTeacherClasses(&school, current->teacher_id);
                    }
                    break;
                }
                case 2: {
                    char className[MAX_NAME_LENGTH];
                    printf("Enter class name: ");
                    scanf(" %[^\n]", className);
                    if (createClass(&school, className, current->teacher_id)) {
                        printf("Class created successfully.\n");
                    } else {
                        printf("Failed to create class. Please try again.\n");
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
                case 4:
                    logout();
                    printf("Logged out successfully.\n");
                    break;
                case 5:
                    printf("Exiting program.\n");
                    break;
                default:
                    printf("Invalid choice. Please try again.\n");
            }
        }
    } while ((isLoggedIn() && choice != 5) || (!isLoggedIn() && choice != 3));

    saveSchoolSystem(&school);
    return 0;
}