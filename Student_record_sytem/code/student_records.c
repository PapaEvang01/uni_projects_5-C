/*
==============================================
   Student Record Management System - C
==============================================

A terminal-based tool to manage student records:
- Add new students (first name, last name, roll number)
- Display all students in a formatted table
- Search by roll number or last name
- Delete student records
- Export data to CSV for use in Excel
- Data is saved persistently in students.dat

Author: Vaggelis Papaioannou

To compile and run:
-----------------------------------
cd C:\
gcc -o student_records student_records.c
.\student_records
-----------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Constants
#define FILE_NAME "students.dat"
#define MAX_STUDENTS 100

// Structure for student record
struct Student {
    char firstName[50];
    char lastName[50];
    char roll[10];  // Roll number as string
};

// Global student list (in-memory)
struct Student students[MAX_STUDENTS];
int studentCount = 0;  // In-memory count

// Function declarations
void addStudent();
void displayStudents();
void searchStudent();
void deleteStudent();
void menu();

int main() {
    menu();
    return 0;
}

// Menu interface for user options
void menu() {
    int choice;

    while (1) {
        printf("\n========== Student Record Management ==========\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by Roll Number\n");
        printf("4. Delete Student\n");
        printf("5. Exit\n");
        printf("6. Export Students to CSV\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: deleteStudent(); break;
            case 5: printf("Exiting...\n"); exit(0);
            case 6: exportToCSV(); break;
            default: printf("Invalid choice. Try again.\n");
        }
    }
}

// Check if roll number matches the format AM12345
int isValidRoll(const char *roll) {
    // Must be exactly 7 characters: "AM" + 5 digits
    if (strlen(roll) != 7) return 0;
    if (roll[0] != 'A' || roll[1] != 'M') return 0;

    for (int i = 2; i < 7; i++) {
        if (!isdigit(roll[i])) return 0;
    }

    return 1;
}

// Add a new student to the file
void addStudent() {
    struct Student s;

    // Input
    printf("Enter First Name: ");
    scanf("%s", s.firstName);

    printf("Enter Last Name: ");
    scanf("%s", s.lastName);

    printf("Enter Roll Number (e.g., AM12345): ");
    scanf("%s", s.roll);

    // Validate roll number format
    if (!isValidRoll(s.roll)) {
        printf(" Invalid Roll Number. It must be in the form AM12345.\n");
        printf(" Format: AM followed by 5 digits. It should be on your email. Please check it.\n");
        return;
    }

    // Check for duplicate roll number in file
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp) {
        struct Student temp;
        while (fread(&temp, sizeof(temp), 1, fp)) {
            if (strcmp(temp.roll, s.roll) == 0) {
                printf(" Student with roll number %s already exists. Cannot add duplicate.\n", s.roll);
                fclose(fp);
                return;
            }
        }
        fclose(fp);
    }

    // Add to memory
    if (studentCount < MAX_STUDENTS) {
        students[studentCount++] = s;
    } else {
        printf(" Maximum number of students reached in memory.\n");
        return;
    }

    // Save to file
    fp = fopen(FILE_NAME, "ab");
    if (!fp) {
        printf("Error opening file!\n");
        return;
    }

    fwrite(&s, sizeof(s), 1, fp);
    fclose(fp);

    printf(" Student added successfully!\n");
}


// Display all student records from the file
void displayStudents() {
    struct Student s;
    FILE *fp = fopen(FILE_NAME, "rb");
    int count = 1;

    if (!fp) {
        printf("No records found.\n");
        return;
    }

    printf("\n%-5s %-18s %-18s %-15s\n", "No", "First Name", "Last Name", "Roll");
    printf("---------------------------------------------------------------\n");

    while (fread(&s, sizeof(s), 1, fp)) {
        printf("%-5d %-20s %-20s %-10s\n", count++, s.firstName, s.lastName, s.roll);
    }

    fclose(fp);
}

// Search student by roll number or last name
void searchStudent() {
    struct Student s;
    int choice;
    int attempts = 0;
    int found = 0;

    printf("\nSearch by:\n");
    printf("1. Roll Number\n");
    printf("2. Last Name\n");
    printf("Enter choice (1 or 2): ");
    scanf("%d", &choice);

    if (choice == 1) {
        char roll[10];
        while (attempts < 3 && !found) {
            FILE *fp = fopen(FILE_NAME, "rb");
            if (!fp) {
                printf("No records found.\n");
                return;
            }

            printf("Enter roll number: ");
            scanf("%s", roll);

            while (fread(&s, sizeof(s), 1, fp)) {
                if (strcmp(s.roll, roll) == 0) {
                    printf("\n Student found:\n");
                    printf("First Name: %s\n", s.firstName);
                    printf("Last Name: %s\n", s.lastName);
                    printf("Roll Number: %s\n", s.roll);
                    found = 1;
                    break;
                }
            }

            fclose(fp);
            if (!found) {
                attempts++;
                if (attempts < 3)
                    printf(" No match. Try again (%d/3 attempts).\n", attempts);
            }
        }

    } else if (choice == 2) {
        char lastName[50];
        while (attempts < 3 && !found) {
            FILE *fp = fopen(FILE_NAME, "rb");
            if (!fp) {
                printf("No records found.\n");
                return;
            }

            printf("Enter last name: ");
            scanf("%s", lastName);

            int matchCount = 0;
            while (fread(&s, sizeof(s), 1, fp)) {
                if (strcmp(s.lastName, lastName) == 0) {
                    if (matchCount == 0)
                        printf("\n Students with last name \"%s\":\n", lastName);
                    printf("--------------------------\n");
                    printf("First Name: %s\n", s.firstName);
                    printf("Last Name: %s\n", s.lastName);
                    printf("Roll Number: %s\n", s.roll);
                    matchCount++;
                    found = 1;
                }
            }

            fclose(fp);

            if (!found) {
                attempts++;
                if (attempts < 3)
                    printf(" No match. Try again (%d/3 attempts).\n", attempts);
            }
        }

    } else {
        printf(" Invalid choice.\n");
        return;
    }

    if (!found)
        printf(" No student found after 3 attempts. Returning to main menu...\n");
}

// Delete a student by roll number
void deleteStudent() {
    struct Student s;
    char roll[20];
    int found = 0;

    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (!fp || !temp) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter roll number to delete: ");
    scanf("%s", roll);

    while (fread(&s, sizeof(s), 1, fp)) {
        if (strcmp(s.roll, roll) == 0) {
            found = 1;
            continue;
        }
        fwrite(&s, sizeof(s), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.dat", FILE_NAME);

    if (found)
        printf(" Student record deleted successfully!\n");
    else
        printf("Student with roll number %s not found.\n", roll);
}

// Export all student records to a CSV file
void exportToCSV() {
    struct Student s;
    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *csv = fopen("students_export.csv", "w");

    if (!fp || !csv) {
        printf(" Error opening file(s) for export.\n");
        return;
    }
    // CSV header
    fprintf(csv, "Roll,First Name,Last Name\n");  // Header

    // Write each student record to CSV
    while (fread(&s, sizeof(s), 1, fp)) {
        fprintf(csv, "%s,%s,%s\n", s.roll, s.firstName, s.lastName);
    }

    fclose(fp);
    fclose(csv);

    printf(" Exported successfully to students_export.csv\n");
}
