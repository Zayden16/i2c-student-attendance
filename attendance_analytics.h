#ifndef ATTENDANCE_ANALYTICS_H
#define ATTENDANCE_ANALYTICS_H

#include "student_library.h"
#include <time.h>

#define MAX_ATTENDANCE_RECORDS 100
#define MAX_DATES 50

typedef struct {
    int studentId;
    time_t date;
    bool present;
} AttendanceRecord;

typedef struct {
    int classId;
    AttendanceRecord records[MAX_ATTENDANCE_RECORDS];
    int recordCount;
    time_t dates[MAX_DATES];
    int dateCount;
} AttendanceHistory;

// Analytics functions
float calculateStudentAttendanceRate(AttendanceHistory* history, int studentId);
float calculateClassAttendanceRate(AttendanceHistory* history);
void identifyChronicAbsentees(AttendanceHistory* history, Class* class, float threshold);
void generateMonthlyReport(AttendanceHistory* history, Class* class, int month, int year);
void generateTrendAnalysis(AttendanceHistory* history, Class* class);
void generateStudentDetailedReport(AttendanceHistory* history, Class* class, int studentId);

// Record management
void addAttendanceRecord(AttendanceHistory* history, int studentId, bool present);
void initializeAttendanceHistory(AttendanceHistory* history, int classId);
bool saveAttendanceHistory(AttendanceHistory* history, const char* filename);
bool loadAttendanceHistory(AttendanceHistory* history, const char* filename);

// Date utilities
void getDateString(time_t date, char* buffer, size_t size);
time_t parseDate(const char* dateStr);
int getDayOfWeek(time_t date);

// Statistical analysis
typedef struct {
    float mean;
    float median;
    float standardDeviation;
    int consecutiveAbsences;
    int totalAbsences;
    int lateArrivals;  // For future implementation of time tracking
} StudentStatistics;

StudentStatistics calculateStudentStatistics(AttendanceHistory* history, int studentId);

// Trend analysis
typedef struct {
    float weekdayRates[7];  // Attendance rates for each day of the week
    float monthlyRates[12]; // Attendance rates for each month
    float overallTrend;     // Positive or negative trend indicator
} TrendAnalysis;

TrendAnalysis analyzeTrends(AttendanceHistory* history, Class* class);

#endif // ATTENDANCE_ANALYTICS_H