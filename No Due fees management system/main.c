#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <conio.h> 

// Define limits and filenames used by the program
#define MAX_STUDENTS 100
#define MAX_NAME_LENGTH 50
#define FILENAME "student.txt"
#define APPROVAL_FILENAME "approval_list.txt"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "admin123"

// Student record structure: one per student
typedef struct {
    int rollNumber;                     // unique roll number
    char name[MAX_NAME_LENGTH];         // student name (C-string)
    float feesDue;                      // outstanding fees amount
    int libraryBooksDue;                // number of library books not returned
    float hostelDue;                    // outstanding hostel dues
    int approvalStatus;                 // 0 = Pending, 1 = Approved
} Student;

// In-memory storage for student records and count
Student students[MAX_STUDENTS];
int studentCount = 0;


// Function prototypes (each function handles a specific feature)
void loadStudentData();
void saveAllStudents();
void displayMainMenu();
void studentMenu();
void adminMenu();
bool authenticateAdmin();
void viewApprovalStatus();
void applyForApproval();
void saveApprovalRequest(int rollNumber);
int isDuplicateApproval(int rollNumber);
void displayAllStudents();
void viewPendingApprovals();
void processApprovals();
void updateStudentRecord();
void addNewStudent();
int getValidIntegerInput(const char *prompt);
float getValidFloatInput(const char *prompt);
void clearInputBuffer();
int isRollNumberExists(int rollNumber);
void printHeader(const char *title);

int main() {
    // Load records from disk at startup (if available)
    loadStudentData();
    
    int choice;
    do {
        displayMainMenu();
        choice = getValidIntegerInput("Enter your choice: ");
        
        switch(choice) {
            case 1:
                studentMenu();   // Student-facing features
                break;
            case 2:
                if (authenticateAdmin()) { // simple admin check
                    adminMenu();
                } else {
                    printf("Authentication failed. Access denied.\n");
                }
                break;
            case 3:
                printf("Exiting the program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while(choice != 3);
    
    return 0;
}

// Load student records from the CSV file into memory
void loadStudentData() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        // No file yet: inform user and continue with empty list
        printf("Warning: Could not open %s file. A new one will be created.\n", FILENAME);
        return;
    }
    
    studentCount = 0;
    // Read each line: roll,name,fees,books,hostel,approval
    while (fscanf(file, "%d,%49[^,],%f,%d,%f,%d",
                 &students[studentCount].rollNumber,
                 students[studentCount].name,
                 &students[studentCount].feesDue,
                 &students[studentCount].libraryBooksDue,
                 &students[studentCount].hostelDue,
                 &students[studentCount].approvalStatus) == 6) {
        studentCount++;
        if (studentCount >= MAX_STUDENTS) {
            // Prevent overflow of our in-memory array
            printf("Warning: Maximum student capacity reached. Some records may not be loaded.\n");
            break;
        }
    }
    
    fclose(file);
    
    if (studentCount == 0) {
        printf("No student records found in %s.\n", FILENAME);
    } else {
        printf("Successfully loaded %d student records.\n", studentCount);
    }
}

// Save all in-memory student records back to the CSV file (overwrite)
void saveAllStudents() {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error: Could not open student file for writing.\n");
        return;
    }
    
    // Write each student as a CSV line
    for (int i = 0; i < studentCount; i++) {
        fprintf(file, "%d,%s,%.2f,%d,%.2f,%d\n",
               students[i].rollNumber,
               students[i].name,
               students[i].feesDue,
               students[i].libraryBooksDue,
               students[i].hostelDue,
               students[i].approvalStatus);
    }
    
    fclose(file);
}

// Display the main menu with options for student and admin
void displayMainMenu() {
    printHeader("Student Approval Management System");
    printf("1. Student Portal\n");
    printf("2. Admin Portal\n");
    printf("3. Exit\n");
    printf("========================================\n");
}

// Menu for student-specific actions
void studentMenu() {
    int choice;
    do {
        printHeader("Student Portal");
        printf("1. View Approval Status\n");
        printf("2. Apply for Approval\n");
        printf("3. Back to Main Menu\n");
        choice = getValidIntegerInput("Enter your choice: ");
        
        switch(choice) {
            case 1:
                viewApprovalStatus();
                break;
            case 2:
                applyForApproval();
                break;
            case 3:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while(choice != 3);
}

// Menu for admin-specific actions
void adminMenu() {
    int choice;
    do {
        printHeader("Admin Portal");
        printf("1. View All Students\n");
        printf("2. View Pending Approvals\n");
        printf("3. Process Approvals\n");
        printf("4. Update Student Record\n");
        printf("5. Add New Student\n");
        printf("6. Back to Main Menu\n");
        choice = getValidIntegerInput("Enter your choice: ");
        
        switch(choice) {
            case 1:
                displayAllStudents();
                break;
            case 2:
                viewPendingApprovals();
                break;
            case 3:
                processApprovals();
                break;
            case 4:
                updateStudentRecord();
                break;
            case 5:
                addNewStudent();
                break;
            case 6:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while(choice != 6);
}

// Simple admin authentication reading username and password (password masked)
bool authenticateAdmin() {
    char username[50];
    char password[50];
    
    printHeader("Admin Login");
    printf("Username: ");
    scanf("%49s", username);
    clearInputBuffer(); // flush leftover newline after scanf
    
    printf("Password: ");
    
    // Read password using getch and display '*' for each character
    int i = 0;
    char ch;
    while ((ch = getch()) != '\r' && ch != '\n' && i < 49) {
        password[i++] = ch;
        printf("*");
    }
    password[i] = '\0';
    printf("\n");
    
    // Compare with stored admin credentials
    return (strcmp(username, ADMIN_USERNAME) == 0 && 
            strcmp(password, ADMIN_PASSWORD) == 0);
}

// Show details and approval status for a given roll number
void viewApprovalStatus() {
    int rollNumber = getValidIntegerInput("Enter your roll number: ");
    
    int found = 0;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == rollNumber) {
            found = 1;
            printHeader("Student Details");
            printf("Roll Number: %d\n", students[i].rollNumber);
            printf("Name: %s\n", students[i].name);
            printf("Fees Due: %.2f\n", students[i].feesDue);
            printf("Library Books Due: %d\n", students[i].libraryBooksDue);
            printf("Hostel Due: %.2f\n", students[i].hostelDue);
            printf("Approval Status: %s\n", students[i].approvalStatus ? "Approved" : "Pending");
            break;
        }
    }
    
    if (!found) {
        printf("Student with roll number %d not found.\n", rollNumber);
    }
}

// Student applies for approval: check eligibility and prevent duplicates
void applyForApproval() {
    displayAllStudents(); // show records so student can find their roll
    
    int rollNumber = getValidIntegerInput("\nEnter your roll number to apply for approval: ");
    
    int found = 0;
    int canApply = 0;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == rollNumber) {
            found = 1;
            if (students[i].approvalStatus == 0) {
                canApply = 1;
                // Inform student they are eligible to submit a request
                printf("\n%s (Roll No: %d), you are eligible to apply for approval.\n", 
                      students[i].name, students[i].rollNumber);
                
                // Prevent duplicate entries in approval file
                if (isDuplicateApproval(rollNumber)) {
                    printf("You have already applied for approval. Duplicate applications are not allowed.\n");
                    return;
                }
                
                saveApprovalRequest(rollNumber); // append request to approval file
                printf("Your approval request has been submitted successfully.\n");
            } else {
                printf("Your approval has already been granted. No need to apply again.\n");
            }
            break;
        }
    }
    
    if (!found) {
        printf("Student with roll number %d not found.\n", rollNumber);
    } else if (!canApply) {
        printf("You are not eligible to apply for approval or your status is already approved.\n");
    }
}

// Append an approval request to APPROVAL_FILENAME
void saveApprovalRequest(int rollNumber) {
    FILE *file = fopen(APPROVAL_FILENAME, "a");
    if (file == NULL) {
        printf("Error: Could not open approval file for writing.\n");
        return;
    }
    
    // Write roll and name for admin processing later
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == rollNumber) {
            fprintf(file, "%d,%s\n", rollNumber, students[i].name);
            break;
        }
    }
    
    fclose(file);
}

// Check if a roll number already exists in the pending approval file
int isDuplicateApproval(int rollNumber) {
    FILE *file = fopen(APPROVAL_FILENAME, "r");
    if (file == NULL) {
        return 0; // no file => no duplicates
    }
    
    int existingRoll;
    char name[MAX_NAME_LENGTH];
    while (fscanf(file, "%d,%49[^\n]", &existingRoll, name) == 2) {
        if (existingRoll == rollNumber) {
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

// Display all students in a tabular form (simple console layout)
void displayAllStudents() {
    printHeader("All Student Records");
    printf("Roll No\tName\t\tFees Due\tBooks Due\tHostel Due\tApproval Status\n");
    printf("-------\t----\t\t--------\t---------\t---------\t---------------\n");
    
    for (int i = 0; i < studentCount; i++) {
        printf("%d\t%-12s\t%.2f\t\t%d\t\t%.2f\t\t%s\n", 
              students[i].rollNumber,
              students[i].name,
              students[i].feesDue,
              students[i].libraryBooksDue,
              students[i].hostelDue,
              students[i].approvalStatus ? "Approved" : "Pending");
    }
}

// List pending approvals by reading the approval file
void viewPendingApprovals() {
    FILE *file = fopen(APPROVAL_FILENAME, "r");
    if (file == NULL) {
        printf("No pending approval requests found.\n");
        return;
    }
    
    printHeader("Pending Approval Requests");
    printf("Roll No\tName\n");
    printf("-------\t----\n");
    
    int rollNumber;
    char name[MAX_NAME_LENGTH];
    int count = 0;
    
    // For each request, show only if student still exists and is pending
    while (fscanf(file, "%d,%49[^\n]", &rollNumber, name) == 2) {
        for (int i = 0; i < studentCount; i++) {
            if (students[i].rollNumber == rollNumber && students[i].approvalStatus == 0) {
                printf("%d\t%s\n", rollNumber, name);
                count++;
                break;
            }
        }
    }
    
    fclose(file);
    
    if (count == 0) {
        printf("No pending approval requests found.\n");
    }
}

// Admin processing of approval requests: approve/reject/skip each entry
void processApprovals() {
    viewPendingApprovals();
    
    // Use a temporary file to keep skipped requests
    FILE *tempFile = fopen("temp_approval.txt", "w");
    FILE *approvalFile = fopen(APPROVAL_FILENAME, "r");
    
    if (approvalFile == NULL) {
        printf("No approval requests to process.\n");
        return;
    }
    
    int rollNumber;
    char name[MAX_NAME_LENGTH];
    int processed = 0;
    
    while (fscanf(approvalFile, "%d,%49[^\n]", &rollNumber, name) == 2) {
        int found = 0;
        for (int i = 0; i < studentCount; i++) {
            if (students[i].rollNumber == rollNumber) {
                found = 1;
                printHeader("Processing Approval");
                printf("Student: %s (Roll No: %d)\n", students[i].name, rollNumber);
                printf("Current status: %s\n", students[i].approvalStatus ? "Approved" : "Pending");
                printf("Fees Due: %.2f\n", students[i].feesDue);
                printf("Library Books Due: %d\n", students[i].libraryBooksDue);
                printf("Hostel Due: %.2f\n", students[i].hostelDue);
                
                int decision;
                do {
                    printf("\n1. Approve\n2. Reject\n3. Skip\nEnter decision: ");
                    decision = getValidIntegerInput("");
                } while (decision < 1 || decision > 3);
                
                if (decision == 1) {
                    // Mark approved in memory; will be saved to disk later
                    students[i].approvalStatus = 1;
                    processed++;
                    printf("Approved successfully.\n");
                } else if (decision == 2) {
                    // Rejected: do nothing to student status (removes request)
                    printf("Application rejected.\n");
                } else {
                    // Skip: preserve request by writing it to the temp file
                    fprintf(tempFile, "%d,%s\n", rollNumber, name);
                }
                break;
            }
        }
        
        if (!found) {
            // Invalid request for a non-existing student: inform and drop it
            printf("Student with roll number %d not found in records. Removing invalid request.\n", rollNumber);
        }
    }
    
    fclose(approvalFile);
    fclose(tempFile);
    
    // Replace old approval file with the temp file that contains skipped requests
    remove(APPROVAL_FILENAME);
    rename("temp_approval.txt", APPROVAL_FILENAME);
    
    // Save any changes made to student statuses
    saveAllStudents();
    
    printf("\nProcessing complete. %d applications were approved.\n", processed);
}

// Update an existing student's numeric fields or approval flag
void updateStudentRecord() {
    displayAllStudents();
    
    int rollNumber = getValidIntegerInput("\nEnter roll number to update: ");
    int found = -1;
    
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == rollNumber) {
            found = i;
            break;
        }
    }
    
    if (found == -1) {
        printf("Student with roll number %d not found.\n", rollNumber);
        return;
    }
    
    printHeader("Update Student Record");
    printf("Current details for %s (Roll No: %d):\n", students[found].name, rollNumber);
    printf("1. Fees Due: %.2f\n", students[found].feesDue);
    printf("2. Library Books Due: %d\n", students[found].libraryBooksDue);
    printf("3. Hostel Due: %.2f\n", students[found].hostelDue);
    printf("4. Approval Status: %s\n", students[found].approvalStatus ? "Approved" : "Pending");
    printf("0. Cancel Update\n");
    
    int choice;
    do {
        choice = getValidIntegerInput("\nSelect field to update (0-4): ");
    } while (choice < 0 || choice > 4);
    
    if (choice == 0) {
        printf("Update cancelled.\n");
        return;
    }
    
    switch(choice) {
        case 1:
            students[found].feesDue = getValidFloatInput("Enter new fees due: ");
            break;
        case 2:
            students[found].libraryBooksDue = getValidIntegerInput("Enter new library books due: ");
            break;
        case 3:
            students[found].hostelDue = getValidFloatInput("Enter new hostel due: ");
            break;
        case 4:
            students[found].approvalStatus = getValidIntegerInput("Enter new approval status (1=Approved, 0=Pending): ");
            break;
    }
    
    saveAllStudents(); // persist changes to disk
    printf("Record updated successfully.\n");
}

// Add a new student: ensure unique roll number and collect fields
void addNewStudent() {
    if (studentCount >= MAX_STUDENTS) {
        printf("Maximum student capacity reached. Cannot add more students.\n");
        return;
    }

    printHeader("Add New Student");
    
    int rollNumber;
    do {
        rollNumber = getValidIntegerInput("Enter roll number: ");
        if (isRollNumberExists(rollNumber)) {
            printf("Error: Roll number already exists. Please enter a unique roll number.\n");
        }
    } while (isRollNumberExists(rollNumber));
    
    Student newStudent;
    newStudent.rollNumber = rollNumber;
    
    // Read full line for student name (may contain spaces)
    printf("Enter student name: ");
    fgets(newStudent.name, MAX_NAME_LENGTH, stdin);
    newStudent.name[strcspn(newStudent.name, "\n")] = '\0'; 
    
    newStudent.feesDue = getValidFloatInput("Enter fees due: ");
    newStudent.libraryBooksDue = getValidIntegerInput("Enter number of library books due: ");
    newStudent.hostelDue = getValidFloatInput("Enter hostel due: ");
    newStudent.approvalStatus = getValidIntegerInput("Enter approval status (1=Approved, 0=Pending): ");
    
    // Add to in-memory array
    students[studentCount++] = newStudent;
    
    // Append new student to the student file for persistence
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf("Error: Could not open student file for writing.\n");
        return;
    }
    
    fprintf(file, "%d,%s,%.2f,%d,%.2f,%d\n",
           newStudent.rollNumber,
           newStudent.name,
           newStudent.feesDue,
           newStudent.libraryBooksDue,
           newStudent.hostelDue,
           newStudent.approvalStatus);
    
    fclose(file);
    
    printf("Student added successfully!\n");
}

// Check in-memory roll numbers for duplicates
int isRollNumberExists(int rollNumber) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == rollNumber) {
            return 1;
        }
    }
    return 0;
}

// Get a validated integer from the user (re-prompts until valid)
int getValidIntegerInput(const char *prompt) {
    int value;
    char buffer[100];
    
    while (1) {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
        
        printf("Invalid input. Please enter a valid integer.\n");
    }
}

// Get a validated floating-point number from the user
float getValidFloatInput(const char *prompt) {
    float value;
    char buffer[100];
    
    while (1) {
        printf("%s", prompt);
        fgets(buffer, sizeof(buffer), stdin);
        
        if (sscanf(buffer, "%f", &value) == 1) {
            return value;
        }
        
        printf("Invalid input. Please enter a valid number.\n");
    }
}

// Flush leftover characters in stdin (used after scanf)
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Simple header printer to separate sections visually
void printHeader(const char *title) {
    printf("\n========================================\n");
    printf("%s\n", title);
    printf("========================================\n");
}