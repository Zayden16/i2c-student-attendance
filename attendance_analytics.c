#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "attendance_analytics.h"

void initializeAttendanceHistory(AttendanceHistory* history, int classId) {
    history->classId = classId;
    history->recordCount = 0;
    history->dateCount = 0;
}

void addAttendanceRecord(AttendanceHistory* history, int studentId, bool present) {
    if (history->recordCount >= MAX_ATTENDANCE_RECORDS) {
        printf("Maximum attendance records reached.\n");
        return;
    }

    time_t now;
    time(&now);
    
    // Check if we already have this date
    bool dateExists = false;
    for (int i = 0; i < history->dateCount; i++) {
        if (difftime(history->dates[i], now) == 0) {
            dateExists = true;
            break;
        }
    }
    
    if (!dateExists && history->dateCount < MAX_DATES) {
        history->dates[history->dateCount++] = now;
    }

    AttendanceRecord* record = &history->records[history->recordCount++];
    record->studentId = studentId;
    record->date = now;
    record->present = present;
}

float calculateStudentAttendanceRate(AttendanceHistory* history, int studentId) {
    int totalRecords = 0;
    int presentCount = 0;

    for (int i = 0; i < history->recordCount; i++) {
        if (history->records[i].studentId == studentId) {
            totalRecords++;
            if (history->records[i].present) {
                presentCount++;
            }
        }
    }

    return totalRecords > 0 ? (float)presentCount / totalRecords * 100 : 0;
}

float calculateClassAttendanceRate(AttendanceHistory* history) {
    int totalRecords = history->recordCount;
    int presentCount = 0;

    for (int i = 0; i < totalRecords; i++) {
        if (history->records[i].present) {
            presentCount++;
        }
    }

    return totalRecords > 0 ? (float)presentCount / totalRecords * 100 : 0;
}

void identifyChronicAbsentees(AttendanceHistory* history, Class* class, float threshold) {
    printf("\nChronic Absentee Report (Below %.1f%% attendance):\n", threshold);
    printf("------------------------------------------------\n");
    printf("ID\tName\t\tAttendance Rate\tConsecutive Absences\n");

    for (int i = 0; i < class->studentCount; i++) {
        StudentStatistics stats = calculateStudentStatistics(history, class->students[i].id);
        float rate = calculateStudentAttendanceRate(history, class->students[i].id);
        
        if (rate < threshold) {
            printf("%d\t%-16s%.1f%%\t\t%d\n",
                   class->students[i].id,
                   class->students[i].name,
                   rate,
                   stats.consecutiveAbsences);
        }
    }
}

StudentStatistics calculateStudentStatistics(AttendanceHistory* history, int studentId) {
    StudentStatistics stats = {0};
    int recordCount = 0;
    float* attendanceValues = malloc(history->recordCount * sizeof(float));
    
    // Collect all attendance values for this student
    for (int i = 0; i < history->recordCount; i++) {
        if (history->records[i].studentId == studentId) {
            attendanceValues[recordCount++] = history->records[i].present ? 1.0f : 0.0f;
        }
    }

    if (recordCount > 0) {
        // Calculate mean
        float sum = 0;
        for (int i = 0; i < recordCount; i++) {
            sum += attendanceValues[i];
        }
        stats.mean = sum / recordCount;

        // Calculate standard deviation
        float sumSquareDiff = 0;
        for (int i = 0; i < recordCount; i++) {
            float diff = attendanceValues[i] - stats.mean;
            sumSquareDiff += diff * diff;
        }
        stats.standardDeviation = sqrt(sumSquareDiff / recordCount);

        // Count consecutive absences
        int currentConsecutive = 0;
        for (int i = 0; i < recordCount; i++) {
            if (attendanceValues[i] == 0) {
                currentConsecutive++;
                if (currentConsecutive > stats.consecutiveAbsences) {
                    stats.consecutiveAbsences = currentConsecutive;
                }
            } else {
                currentConsecutive = 0;
            }
        }

        // Count total absences
        stats.totalAbsences = recordCount - (int)(sum);
    }

    free(attendanceValues);
    return stats;
}

TrendAnalysis analyzeTrends(AttendanceHistory* history, Class* class) {
    TrendAnalysis trends = {0};
    int weekdayCounts[7] = {0};
    int monthlyCounts[12] = {0};
    int weekdayPresent[7] = {0};
    int monthlyPresent[12] = {0};

    // Calculate attendance rates by day and month
    for (int i = 0; i < history->recordCount; i++) {
        time_t date = history->records[i].date;
        struct tm* timeinfo = localtime(&date);
        
        int weekday = timeinfo->tm_wday;
        int month = timeinfo->tm_mon;
        
        weekdayCounts[weekday]++;
        monthlyCounts[month]++;
        
        if (history->records[i].present) {
            weekdayPresent[weekday]++;
            monthlyPresent[month]++;
        }
    }

    // Calculate rates
    for (int i = 0; i < 7; i++) {
        trends.weekdayRates[i] = weekdayCounts[i] > 0 ?
            (float)weekdayPresent[i] / weekdayCounts[i] * 100 : 0;
    }

    for (int i = 0; i < 12; i++) {
        trends.monthlyRates[i] = monthlyCounts[i] > 0 ?
            (float)monthlyPresent[i] / monthlyCounts[i] * 100 : 0;
    }

    // Calculate overall trend
    if (history->recordCount > 0) {
        float firstHalfAvg = 0, secondHalfAvg = 0;
        int midPoint = history->recordCount / 2;
        
        for (int i = 0; i < midPoint; i++) {
            firstHalfAvg += history->records[i].present ? 100 : 0;
        }
        for (int i = midPoint; i < history->recordCount; i++) {
            secondHalfAvg += history->records[i].present ? 100 : 0;
        }
        
        firstHalfAvg /= midPoint;
        secondHalfAvg /= (history->recordCount - midPoint);
        
        trends.overallTrend = secondHalfAvg - firstHalfAvg;
    }

    return trends;
}

void generateMonthlyReport(AttendanceHistory* history, Class* class, int month, int year) {
    printf("\nMonthly Attendance Report - %d/%d\n", month, year);
    printf("----------------------------------------\n");
    
    // Print header with dates
    printf("ID\tName\t\t");
    for (int i = 0; i < history->dateCount; i++) {
        struct tm* timeinfo = localtime(&history->dates[i]);
        if (timeinfo->tm_mon + 1 == month && timeinfo->tm_year + 1900 == year) {
            printf("%d\t", timeinfo->tm_mday);
        }
    }
    printf("Rate\n");

    // Print attendance data for each student
    for (int i = 0; i < class->studentCount; i++) {
        Student* student = &class->students[i];
        printf("%d\t%-16s", student->id, student->name);
        
        int presentCount = 0;
        int totalDays = 0;
        
        for (int j = 0; j < history->recordCount; j++) {
            if (history->records[j].studentId == student->id) {
                struct tm* timeinfo = localtime(&history->records[j].date);
                if (timeinfo->tm_mon + 1 == month && timeinfo->tm_year + 1900 == year) {
                    printf("%c\t", history->records[j].present ? 'P' : 'A');
                    if (history->records[j].present) presentCount++;
                    totalDays++;
                }
            }
        }
        
        float rate = totalDays > 0 ? (float)presentCount / totalDays * 100 : 0;
        printf("%.1f%%\n", rate);
    }
}

void generateStudentDetailedReport(AttendanceHistory* history, Class* class, int studentId) {
    // Find student
    Student* student = NULL;
    for (int i = 0; i < class->studentCount; i++) {
        if (class->students[i].id == studentId) {
            student = &class->students[i];
            break;
        }
    }
    
    if (student == NULL) {
        printf("Student not found.\n");
        return;
    }

    StudentStatistics stats = calculateStudentStatistics(history, studentId);
    TrendAnalysis trends = analyzeTrends(history, class);

    printf("\nDetailed Attendance Report for %s (ID: %d)\n", student->name, student->id);
    printf("================================================\n");
    
    // Overall statistics
    printf("\nOverall Statistics:\n");
    printf("Total Days: %d\n", history->dateCount);
    printf("Days Present: %d\n", history->dateCount - stats.totalAbsences);
    printf("Days Absent: %d\n", stats.totalAbsences);
    printf("Attendance Rate: %.1f%%\n", stats.mean * 100);
    printf("Longest Consecutive Absences: %d\n", stats.consecutiveAbsences);
    
    // Trend analysis
    printf("\nAttendance by Day of Week:\n");
    const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    for (int i = 0; i < 7; i++) {
        printf("%s: %.1f%%\n", days[i], trends.weekdayRates[i]);
    }
    
    // Monthly patterns
    printf("\nMonthly Attendance Patterns:\n");
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; i++) {
        if (trends.monthlyRates[i] > 0) {
            printf("%s: %.1f%%\n", months[i], trends.monthlyRates[i]);
        }
    }
    
    // Trend indication
    printf("\nAttendance Trend: ");
    if (trends.overallTrend > 5) {
        printf("Improving (↑%.1f%%)\n", trends.overallTrend);
    } else if (trends.overallTrend < -5) {
        printf("Declining (↓%.1f%%)\n", -trends.overallTrend);
    } else {
        printf("Stable\n");
    }
}
bool saveAttendanceHistory(AttendanceHistory* history, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return false;
    }

    fprintf(file, "%d\n%d\n%d\n", history->classId, history->recordCount, history->dateCount);

    // Save dates
    for (int i = 0; i < history->dateCount; i++) {
        fprintf(file, "%ld\n", history->dates[i]);
    }

    // Save attendance records
    for (int i = 0; i < history->recordCount; i++) {
        AttendanceRecord* record = &history->records[i];
        fprintf(file, "%d,%ld,%d\n",
                record->studentId,
                record->date,
                record->present ? 1 : 0);
    }

    fclose(file);
    return true;
}

bool loadAttendanceHistory(AttendanceHistory* history, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return false;
    }

    if (fscanf(file, "%d\n%d\n%d\n",
               &history->classId,
               &history->recordCount,
               &history->dateCount) != 3) {
        fclose(file);
        return false;
    }

    // Load dates
    for (int i = 0; i < history->dateCount; i++) {
        if (fscanf(file, "%ld\n", &history->dates[i]) != 1) {
            fclose(file);
            return false;
        }
    }

    // Load attendance records
    for (int i = 0; i < history->recordCount; i++) {
        AttendanceRecord* record = &history->records[i];
        int present;
        if (fscanf(file, "%d,%ld,%d\n",
                   &record->studentId,
                   &record->date,
                   &present) != 3) {
            fclose(file);
            return false;
        }
        record->present = present != 0;
    }

    fclose(file);
    return true;
}

void getDateString(time_t date, char* buffer, size_t size) {
    struct tm* timeinfo = localtime(&date);
    strftime(buffer, size, "%Y-%m-%d", timeinfo);
}

time_t parseDate(const char* dateStr) {
    struct tm timeinfo = {0};
    if (sscanf(dateStr, "%d-%d-%d",
               &timeinfo.tm_year,
               &timeinfo.tm_mon,
               &timeinfo.tm_mday) != 3) {
        return (time_t)-1;
    }

    timeinfo.tm_year -= 1900;  // Adjust year
    timeinfo.tm_mon -= 1;      // Adjust month (0-11)

    return mktime(&timeinfo);
}

int getDayOfWeek(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo->tm_wday;
}