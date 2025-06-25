/*
===========================================================
    Stylish To-Do List Application in C (Terminal-based)
===========================================================

Description:
------------
This is a terminal-based To-Do List application built in C.
It allows users to manage tasks with features like:

✔ Add new tasks with deadline, priority, and category
✔ Mark tasks as completed
✔ Delete and edit existing tasks
✔ Sort tasks by deadline (earliest first)
✔ Color-coded display for priority, category, and status
✔ Filter tasks by category (Work, Study, Personal, Other)
✔ Highlight overdue tasks
✔ Save/load tasks from a local binary file (tasks.dat)

Color Legend:
-------------
- 🔴 High Priority     → Red
- 🟡 Medium Priority   → Yellow
- 🟣 Low Priority      → Magenta
- 🔵 Study Category    → Blue
- 🟡 Work Category     → Yellow
- 🟣 Personal Category → Magenta
- ⚪ Other Category     → Gray

File Structure:
---------------
- Struct: `Task` holds all task info
- Main Loop: Displays menu, handles user input
- File I/O: Saves/loads tasks on start/exit
- Logic: Deadline parsing, sorting, filtering, coloring

Made for Windows Terminal (requires windows.h for color).

Author: Vaggelis Papaioannou

*/

#include <stdio.h>
#include <string.h>
#include <windows.h>  // For color support on Windows
#include <time.h>

#define MAX_TASKS 100
#define MAX_LENGTH 100

// Windows color codes
#define RESET   15
#define BLUE    9
#define GREEN   10
#define RED     12
#define YELLOW  14
#define MAGENTA 13
#define GRAY    8
#define SAVE_FILE "tasks.dat"

// Task structure
typedef struct {
    char description[MAX_LENGTH];
    char deadline[20];
    int priority;
    int completed;
    char category[20];  // New: Category field
} Task;

// Function declarations
void setColor(int color);
void printHeader();
void addTask(Task tasks[], int *taskCount);
void viewTasks(Task tasks[], int taskCount);
void completeTask(Task tasks[], int taskCount);
void deleteTask(Task tasks[], int *taskCount);
void saveTasks(Task tasks[], int taskCount);
void loadTasks(Task tasks[], int *taskCount);
void editTask(Task tasks[], int taskCount);
int compareDates(const void *a, const void *b);
void parseDate(const char *dateStr, struct tm *date);

int main() {
    Task tasks[MAX_TASKS];      // Array to hold all tasks
    int taskCount = 0;          // Total number of tasks
    int choice;                 // User menu choice

    loadTasks(tasks, &taskCount);  // Load saved tasks from file at program start

    do {
        printHeader();              // Print the stylized header

        // Display menu options
        setColor(BLUE);
        printf("1. Add Task\n");
        printf("2. View Tasks\n");
        printf("3. Mark Task as Completed\n");
        printf("4. Delete Task\n");
        printf("5. Edit Task\n");
        printf("6. Exit\n");
        setColor(YELLOW);
        printf("Choose an option: ");
        setColor(RESET);

        scanf("%d", &choice);
        getchar(); // Clear leftover newline from input buffer

        // Execute action based on user's menu choice
        switch (choice) {
            case 1:
                addTask(tasks, &taskCount);      // Add a new task
                break;
            case 2:
                viewTasks(tasks, taskCount);     // Display all tasks (with filtering)
                break;
            case 3:
                completeTask(tasks, taskCount);  // Mark a task as completed
                break;
            case 4:
                deleteTask(tasks, &taskCount);   // Delete a task by number
                break;
            case 5:
                editTask(tasks, taskCount);      // Edit an existing task
                break;
            case 6:
                saveTasks(tasks, taskCount);     // Save all tasks to file before exiting
                setColor(GREEN);
                printf("Exiting program...\n");
                setColor(RESET);
                break;
            default:
                setColor(RED);
                printf("Invalid choice. Try again.\n"); // Handle invalid input
                setColor(RESET);
        }
    } while (choice != 6);  // Loop until user chooses to exit

    return 0;  // Successful program termination
}


// Sets the text color in the Windows console
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);   // Get handle to standard output
    SetConsoleTextAttribute(hConsole, color);            // Set the text color using color code
}

// Prints a styled header at the top of the menu
void printHeader() {
    setColor(BLUE);  // Set text color to blue
    printf("\n***************************************\n");
    printf("*        Stylish To-Do List Menu       *\n");
    printf("***************************************\n");
    setColor(RESET); // Reset to default console color
}



// Adds a new task to the task list
void addTask(Task tasks[], int *taskCount) {
    // Check if task list has reached its maximum capacity
    if (*taskCount >= MAX_TASKS) {
        setColor(RED);
        printf("Task list is full!\n");
        setColor(RESET);
        return;
    }

    Task newTask;

    // Prompt and read task description
    setColor(YELLOW);
    printf("Enter task description: ");
    setColor(RESET);
    fgets(newTask.description, MAX_LENGTH, stdin);
    newTask.description[strcspn(newTask.description, "\n")] = '\0';  // Remove trailing newline

    // Prompt and read deadline
    printf("Enter deadline (e.g., 2025-06-30): ");
    fgets(newTask.deadline, 20, stdin);
    newTask.deadline[strcspn(newTask.deadline, "\n")] = '\0';

    // Prompt and read priority (1-High, 2-Medium, 3-Low)
    printf("Enter priority (1 = High, 2 = Medium, 3 = Low): ");
    scanf("%d", &newTask.priority);
    getchar();  // Clear newline from buffer

    // Prompt and read task category
    setColor(YELLOW);
    printf("Enter category [Study, Work, Personal, Other]: ");
    setColor(RESET);
    fgets(newTask.category, 20, stdin);
    newTask.category[strcspn(newTask.category, "\n")] = '\0';

    // Initialize task as not completed
    newTask.completed = 0;

    // Add task to the array and increment task count
    tasks[*taskCount] = newTask;
    (*taskCount)++;

    setColor(GREEN);
    printf(" Task added successfully.\n");
    setColor(RESET);
}


// Displays all tasks, optionally filtered by category and sorted by deadline
void viewTasks(Task tasks[], int taskCount) {
    if (taskCount == 0) {
        // No tasks to show
        setColor(RED);
        printf("No tasks to show.\n");
        setColor(RESET);
        return;
    }

    // Ask user to choose a filter by category
    setColor(YELLOW);
    printf("\nView Options:\n");
    printf("1. Work\n2. Study\n3. Personal\n4. Other\n5. All\nChoose filter: ");
    setColor(RESET);

    int filterChoice;
    scanf("%d", &filterChoice);
    getchar();  // Clear newline from buffer

    // Map numeric choice to category string
    char filterCategory[20] = "";
    switch (filterChoice) {
        case 1: strcpy(filterCategory, "Work"); break;
        case 2: strcpy(filterCategory, "Study"); break;
        case 3: strcpy(filterCategory, "Personal"); break;
        case 4: strcpy(filterCategory, "Other"); break;
        case 5: strcpy(filterCategory, "ALL"); break;
        default:
            // Invalid choice, fallback to showing all
            setColor(RED);
            printf("Invalid choice. Showing all tasks.\n");
            setColor(RESET);
            strcpy(filterCategory, "ALL");
    }

    // Sort tasks by deadline (soonest first)
    qsort(tasks, taskCount, sizeof(Task), compareDates);

    // Print column headers
    setColor(BLUE);
    printf("\n%-3s %-30s %-12s %-10s %-10s %-12s %-10s\n", 
           "No", "Description", "Deadline", "Priority", "Status", "Category", "Due In");
    setColor(RESET);

    time_t now = time(NULL);  // Current system time

    for (int i = 0; i < taskCount; i++) {
        // Apply filter: skip task if it doesn't match selected category
        if (strcmp(filterCategory, "ALL") != 0 && strcasecmp(tasks[i].category, filterCategory) != 0)
            continue;

        // Convert deadline string to struct tm
        struct tm taskDate = {0};
        parseDate(tasks[i].deadline, &taskDate);
        time_t taskTime = mktime(&taskDate);

        // Calculate days remaining until deadline
        double daysRemaining = difftime(taskTime, now) / (60 * 60 * 24);
        char dayStr[20];
        if (tasks[i].completed)
            strcpy(dayStr, "—");  // Not applicable for completed tasks
        else if (daysRemaining < 0)
            strcpy(dayStr, "Overdue");  // Task is overdue
        else
            sprintf(dayStr, "%.0f days", daysRemaining);  // Days left

        // Highlight overdue tasks in red
        int isOverdue = (!tasks[i].completed && difftime(taskTime, now) < 0);
        if (isOverdue) setColor(RED);
        else setColor(RESET);

        // Print description and deadline
        printf("%-3d %-30s %-12s ", i + 1, tasks[i].description, tasks[i].deadline);

        // Print and color-code priority
        switch (tasks[i].priority) {
            case 1: setColor(RED);     printf("%-10s ", "High"); break;
            case 2: setColor(YELLOW);  printf("%-10s ", "Medium"); break;
            case 3: setColor(MAGENTA); printf("%-10s ", "Low"); break;
            default: setColor(RESET);  printf("%-10s ", "Unknown");
        }

        // Print completion status
        setColor(tasks[i].completed ? GREEN : RED);
        printf("%-10s ", tasks[i].completed ? " Done" : "Open");

        // Print category with color based on type
        if (strcmp(tasks[i].category, "Study") == 0) setColor(BLUE);
        else if (strcmp(tasks[i].category, "Work") == 0) setColor(YELLOW);
        else if (strcmp(tasks[i].category, "Personal") == 0) setColor(MAGENTA);
        else setColor(GRAY);  // Default color for "Other" or unknown

        printf("%-12s ", tasks[i].category);

        // Print "due in" info
        setColor(isOverdue ? RED : RESET);
        printf("%-10s\n", dayStr);

        setColor(RESET);  // Reset color after each task
    }
}

// Marks a specific task as completed based on user input
void completeTask(Task tasks[], int taskCount) {
    int number;

    // Prompt the user for the task number to mark as completed
    setColor(YELLOW);
    printf("Enter task number to mark as completed: ");
    setColor(RESET);
    scanf("%d", &number);

    // Validate the entered task number
    if (number < 1 || number > taskCount) {
        setColor(RED);
        printf("Invalid task number!\n");  // Error if out of bounds
        setColor(RESET);
        return;
    }

    // Mark the corresponding task as completed
    tasks[number - 1].completed = 1;

    // Confirm to user
    setColor(GREEN);
    printf("Task marked as completed.\n");
    setColor(RESET);
}


// Deletes a task from the list based on the user's input
void deleteTask(Task tasks[], int *taskCount) {
    int number;

    // Prompt user to enter the number of the task they want to delete
    setColor(YELLOW);
    printf("Enter task number to delete: ");
    setColor(RESET);
    scanf("%d", &number);

    // Validate the task number
    if (number < 1 || number > *taskCount) {
        setColor(RED);
        printf("Invalid task number!\n");  // Print error if out of range
        setColor(RESET);
        return;
    }

    // Shift all tasks after the deleted one one position to the left
    for (int i = number - 1; i < *taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];  // Overwrite current task with the next one
    }

    // Reduce the total task count by 1
    (*taskCount)--;

    // Notify the user of successful deletion
    setColor(GREEN);
    printf("Task deleted.\n");
    setColor(RESET);
}


// Saves all tasks to a binary file for persistence between sessions
void saveTasks(Task tasks[], int taskCount) {
    // Open the save file in binary write mode
    FILE *fp = fopen(SAVE_FILE, "wb");
    
    // If file couldn't be opened, print error and exit
    if (!fp) {
        setColor(RED);
        printf("Failed to save tasks.\n");
        setColor(RESET);
        return;
    }

    // First, write the number of tasks to the file
    fwrite(&taskCount, sizeof(int), 1, fp);  // Save the task count

    // Then, write the entire array of tasks
    fwrite(tasks, sizeof(Task), taskCount, fp);  // Save all task data

    // Close the file after writing
    fclose(fp);
}


// Loads tasks from a binary file into memory at program startup
void loadTasks(Task tasks[], int *taskCount) {
    // Open the saved file in binary read mode
    FILE *fp = fopen(SAVE_FILE, "rb");

    // If the file doesn't exist (first run), exit silently
    if (!fp) return;

    // Read the number of tasks first (stored as an integer)
    fread(taskCount, sizeof(int), 1, fp);

    // Read the array of Task structs into memory
    fread(tasks, sizeof(Task), *taskCount, fp);

    // Close the file after loading
    fclose(fp);
}


// Comparison function used by qsort to sort tasks by their deadline date
int compareDates(const void *a, const void *b) {
    // Cast the generic pointers to Task pointers
    const Task *taskA = (const Task *)a;
    const Task *taskB = (const Task *)b;

    // Create tm structs to hold parsed date values
    struct tm dateA = {0}, dateB = {0};

    // Parse each task's deadline string into the tm structs
    parseDate(taskA->deadline, &dateA);
    parseDate(taskB->deadline, &dateB);

    // Convert the tm structs into time_t values (UNIX timestamps)
    time_t timeA = mktime(&dateA);
    time_t timeB = mktime(&dateB);

    // Return the difference between the two deadlines
    // Positive if taskA is later than taskB, negative if earlier
    return difftime(timeA, timeB);  // Ascending order
}


// Function to edit an existing task's details: description, deadline, and priority
void editTask(Task tasks[], int taskCount) {
    // Handle empty task list
    if (taskCount == 0) {
        setColor(RED);
        printf("No tasks available to edit.\n");
        setColor(RESET);
        return;
    }

    // Ask user for task number to edit
    int number;
    setColor(YELLOW);
    printf("Enter task number to edit: ");
    setColor(RESET);
    scanf("%d", &number);
    getchar(); // Clear newline character left in buffer

    // Validate task number input
    if (number < 1 || number > taskCount) {
        setColor(RED);
        printf("Invalid task number!\n");
        setColor(RESET);
        return;
    }

    // Get pointer to the selected task
    Task *t = &tasks[number - 1];

    // Show current description and ask for new one
    setColor(YELLOW);
    printf("Editing Task %d:\n", number);
    printf("Current Description: %s\n", t->description);
    printf("Enter new description (or press Enter to keep): ");
    setColor(RESET);

    char input[MAX_LENGTH];
    fgets(input, MAX_LENGTH, stdin);
    if (strcmp(input, "\n") != 0) {
        input[strcspn(input, "\n")] = '\0';  // Remove newline
        strcpy(t->description, input);      // Update description
    }

    // Update deadline if new input is provided
    setColor(YELLOW);
    printf("Current Deadline: %s\n", t->deadline);
    printf("Enter new deadline (YYYY-MM-DD) (or press Enter to keep): ");
    setColor(RESET);
    fgets(input, 20, stdin);
    if (strcmp(input, "\n") != 0) {
        input[strcspn(input, "\n")] = '\0';
        strcpy(t->deadline, input);
    }

    // Update priority if new valid number is provided
    setColor(YELLOW);
    printf("Current Priority: %d\n", t->priority);
    printf("Enter new priority (1=High, 2=Medium, 3=Low) or 0 to keep: ");
    setColor(RESET);
    int newPriority;
    scanf("%d", &newPriority);
    getchar();  // Clear input buffer again
    if (newPriority >= 1 && newPriority <= 3) {
        t->priority = newPriority;
    }

    setColor(GREEN);
    printf("Task updated successfully!\n");
    setColor(RESET);
}

// Parse "YYYY-MM-DD" into struct tm
void parseDate(const char *dateStr, struct tm *date) {
    sscanf(dateStr, "%d-%d-%d",
           &date->tm_year, &date->tm_mon, &date->tm_mday);
    date->tm_year -= 1900;  // Year since 1900
    date->tm_mon -= 1;      // Month is 0-based
    date->tm_hour = 0;
    date->tm_min = 0;
    date->tm_sec = 0;
}