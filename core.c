#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h> // Needed for thread-safe timer and other Windows features
    #define BROWSER_CMD "start \"\""
#else
    #define BROWSER_CMD "xdg-open"
    // Note: Threading and some system commands would need a POSIX implementation (pthreads)
#endif

#define TASKS_FILE "tasks.txt"
#define LOG_FILE "log.txt"
#define MAX_TASKS 100
#define MAX_LINE_LEN 256

// Data structure for holidays
typedef struct {
    const char* name;
    int day;
    int month;
} Holiday;

// Data structure for passing arguments to the timer thread
typedef struct {
    int duration_seconds;
} TimerArgs;


// --- Function Prototypes ---
void log_command(const char *command, const char *status);
void add_task(const char *task_description);
void view_tasks();
void clear_tasks();
void mark_task_complete(int task_number);
void open_browser_and_search(const char* base_url, const char* query);
void show_time();
char* url_encode(const char* str);
char* get_substring(const char* source, const char* keyword);

// --- New Feature Functions ---
void get_system_usage();
void take_screenshot();
void open_camera(const char* mode);
void open_store_and_search(const char* app_name);
void open_file_explorer();
void show_my_location();
void locate_place(const char* place);
void open_inbox();
void show_next_holiday();
void system_power_control(const char* action);
void launch_application(const char* app_name);
void perform_calculation(const char* expression);
void set_timer(const char* timer_command);
DWORD WINAPI timer_thread(LPVOID lpParam);


int main(int argc, char *argv[]) {
    if (argc < 2) {
        log_command("NULL", "FAILURE (No command passed to C)");
        return 1;
    }

    char full_command[1024] = "";
    for (int i = 1; i < argc; i++) {
        strncat(full_command, argv[i], sizeof(full_command) - strlen(full_command) - 1);
        if (i < argc - 1) {
            strncat(full_command, " ", sizeof(full_command) - strlen(full_command) - 1);
        }
    }
    
    // --- MAIN COMMAND ROUTER ---
    if (strstr(full_command, "task") != NULL) {
        if (strstr(full_command, "view") || strstr(full_command, "show")) view_tasks();
        else if (strstr(full_command, "clear") || strstr(full_command, "delete")) clear_tasks();
        else if (strstr(full_command, "add")) add_task(get_substring(full_command, "task"));
        else if (strstr(full_command, "complete") || strstr(full_command, "mark")) {
            char* task_num_str = get_substring(full_command, "task");
            if (task_num_str) {
                mark_task_complete(atoi(task_num_str));
                free(task_num_str);
            }
        }
    } else if (strstr(full_command, "shutdown")) {
        system_power_control("shutdown");
    } else if (strstr(full_command, "restart")) {
        system_power_control("restart");
    } else if (strstr(full_command, "sleep")) {
        system_power_control("sleep");
    } else if (strstr(full_command, "notepad")) {
        launch_application("notepad");
    } else if (strstr(full_command, "calculator")) {
        launch_application("calculator");
    } else if (strstr(full_command, "command prompt") || strstr(full_command, "cmd")) {
        launch_application("cmd");
    } else if (strstr(full_command, "wikipedia")) {
        char* query = get_substring(full_command, "search");
        if (query && strlen(query) > 0) open_browser_and_search("https://en.wikipedia.org/wiki/", query);
        free(query);
    } else if (strstr(full_command, "calculate")) {
        perform_calculation(get_substring(full_command, "calculate"));
    } else if (strstr(full_command, "timer")) {
        set_timer(get_substring(full_command, "timer"));
    } else if (strstr(full_command, "google") != NULL) {
        char* query = get_substring(full_command, "search");
        if (query && strlen(query) > 0) open_browser_and_search("https://www.google.com/search?q=", query);
        else {
             char command[512];
             snprintf(command, sizeof(command), "%s \"https://www.google.com\"", BROWSER_CMD);
             system(command);
        }
        free(query);
    } else if (strstr(full_command, "youtube") != NULL) {
        char* query = get_substring(full_command, "search");
        if (query && strlen(query) > 0) open_browser_and_search("https://www.youtube.com/results?search_query=", query);
        else {
            char command[512];
            snprintf(command, sizeof(command), "%s \"https://www.youtube.com\"", BROWSER_CMD);
            system(command);
        }
        free(query);
    } else if (strstr(full_command, "time") != NULL) {
        show_time();
    } else if (strstr(full_command, "system") || strstr(full_command, "usage")) {
        get_system_usage();
    } else if (strstr(full_command, "screenshot") != NULL) {
        take_screenshot();
    } else if (strstr(full_command, "photo") != NULL) {
        open_camera("photo");
    } else if (strstr(full_command, "video") != NULL) {
        open_camera("video");
    } else if (strstr(full_command, "store") != NULL) {
        char* app_name = get_substring(full_command, "search");
        open_store_and_search(app_name);
        free(app_name);
    } else if (strstr(full_command, "explorer") != NULL) {
        open_file_explorer();
    } else if (strstr(full_command, "my location") != NULL) {
        show_my_location();
    } else if (strstr(full_command, "locate") != NULL) {
        char* place = get_substring(full_command, "locate");
        locate_place(place);
        free(place);
    } else if (strstr(full_command, "inbox") != NULL) {
        open_inbox();
    } else if (strstr(full_command, "holiday") != NULL) {
        show_next_holiday();
    } else {
        log_command(full_command, "FAILURE (Unrecognized by C)");
    }

    return 0;
}

// ==============================================================================
// --- NEW FEATURE IMPLEMENTATIONS ---
// ==============================================================================

void system_power_control(const char* action) {
    char command[256];
    printf("-> C Program: Performing system action: %s\n", action);
    if (strcmp(action, "shutdown") == 0) {
        system("shutdown /s /t 1");
    } else if (strcmp(action, "restart") == 0) {
        system("shutdown /r /t 1");
    } else if (strcmp(action, "sleep") == 0) {
        system("rundll32.exe powrprof.dll,SetSuspendState 0,1,0");
    }
    log_command(action, "SUCCESS");
}

void launch_application(const char* app_name) {
    printf("-> C Program: Launching %s...\n", app_name);
    if (strcmp(app_name, "notepad") == 0) {
        system("start notepad.exe");
    } else if (strcmp(app_name, "calculator") == 0) {
        system("start calc.exe");
    } else if (strcmp(app_name, "cmd") == 0) {
        system("start cmd.exe");
    }
    log_command(app_name, "SUCCESS (Launched)");
}

void perform_calculation(const char* expression) {
    if (expression == NULL) return;
    
    double num1, num2, result;
    char op_word[20];

    // Try to parse format "number operator number"
    if (sscanf(expression, "%lf %19s %lf", &num1, op_word, &num2) == 3) {
        char op = 0;
        if (strcmp(op_word, "plus") == 0 || strcmp(op_word, "+") == 0) op = '+';
        else if (strcmp(op_word, "minus") == 0 || strcmp(op_word, "-") == 0) op = '-';
        else if (strcmp(op_word, "times") == 0 || strcmp(op_word, "x") == 0 || strcmp(op_word, "*") == 0) op = '*';
        else if (strcmp(op_word, "divided") == 0 || strcmp(op_word, "/") == 0) op = '/';

        switch (op) {
            case '+': result = num1 + num2; break;
            case '-': result = num1 - num2; break;
            case '*': result = num1 * num2; break;
            case '/': 
                if (num2 != 0) result = num1 / num2;
                else { printf("-> C Program: Error! Cannot divide by zero.\n"); return; }
                break;
            default: printf("-> C Program: Unknown operation '%s'.\n", op_word); return;
        }
        printf("\n-> C Program: The result of %s is %.2f\n\n", expression, result);
        log_command(expression, "SUCCESS (Calculated)");
    } else {
        printf("-> C Program: Could not understand the calculation. Please use format: 'number operator number'.\n");
    }
}

// This function runs in a separate thread.
DWORD WINAPI timer_thread(LPVOID lpParam) {
    TimerArgs* args = (TimerArgs*)lpParam;
    int duration = args->duration_seconds;
    free(args); // Free the allocated memory for arguments

    Sleep(duration * 1000);
    printf("\n\n!!! TIMER FINISHED !!!\nTime's up!\n");
    printf("Listening...\r"); // Reprint listening prompt
    fflush(stdout);
    return 0;
}

void set_timer(const char* timer_command) {
    if (timer_command == NULL) return;

    int duration = 0;
    char unit[20];
    if (sscanf(timer_command, "%d %19s", &duration, unit) == 2) {
        if (strstr(unit, "minute") != NULL) {
            duration *= 60; // Convert minutes to seconds
        }
        
        TimerArgs* args = malloc(sizeof(TimerArgs));
        if (args == NULL) {
             printf("-> C Program: Failed to allocate memory for timer.\n");
             return;
        }
        args->duration_seconds = duration;

        // Create a new thread for the timer so it doesn't block the main program
        HANDLE hThread = CreateThread(NULL, 0, timer_thread, args, 0, NULL);
        if (hThread == NULL) {
            printf("-> C Program: Failed to create timer thread.\n");
            free(args);
        } else {
            printf("-> C Program: Timer set for %s. You can continue using other commands.\n", timer_command);
            CloseHandle(hThread); // We don't need to wait for it, so close the handle
        }
    } else {
        printf("-> C Program: Could not understand timer command. Use format: 'set timer for 5 minutes'.\n");
    }
}

void open_inbox() {
    printf("-> C Program: Opening Gmail inbox...\n");
    char command[512];
    snprintf(command, sizeof(command), "%s \"https://mail.google.com/mail/u/0/#inbox\"", BROWSER_CMD);
    system(command);
    log_command("open inbox", "SUCCESS");
}

void show_next_holiday() {
    // List of holidays for 2025
    Holiday holidays[] = {
        {"Republic Day", 26, 1}, {"Holi", 26, 3}, {"Good Friday", 18, 4},
        {"Independence Day", 15, 8}, {"Gandhi Jayanti", 2, 10}, {"Dussehra", 22, 10},
        {"Diwali", 10, 11}, {"Christmas Day", 25, 12}
    };
    int num_holidays = sizeof(holidays) / sizeof(holidays[0]);

    time_t now; time(&now);
    struct tm *local = localtime(&now);
    int current_day = local->tm_mday;
    int current_month = local->tm_mon + 1;
    Holiday next_holiday = {"", -1, -1};
    int found = 0;

    for (int i = 0; i < num_holidays; i++) {
        if (holidays[i].month > current_month || (holidays[i].month == current_month && holidays[i].day > current_day)) {
            next_holiday = holidays[i]; found = 1; break;
        }
    }

    printf("\n-> C Program: Checking for the next holiday...\n");
    if (found) {
        printf("The next holiday is %s on %d/%d/2025.\n\n", next_holiday.name, next_holiday.day, next_holiday.month);
    } else {
        printf("There are no more major holidays scheduled for the rest of the year.\n\n");
    }
    log_command("next holiday", "SUCCESS");
}

void get_system_usage() {
    printf("\n-> C Program: Fetching System Usage...\n");
    printf("-------------------------------------\n");
    printf("CPU Usage (%%): "); fflush(stdout);
    system("powershell \"(Get-WmiObject -Class Win32_Processor).LoadPercentage\"");
    printf("Memory Usage (%%): "); fflush(stdout);
    system("powershell \"$mem = Get-CimInstance Win32_OperatingSystem; [math]::Round((($mem.TotalVisibleMemorySize - $mem.FreePhysicalMemory) / $mem.TotalVisibleMemorySize) * 100)\"");
    printf("Disk Usage (C: %%) : "); fflush(stdout);
    system("powershell \"$disk = Get-CimInstance -ClassName Win32_LogicalDisk -Filter 'DeviceID=\\\"C:\\\"'; [math]::Round((($disk.Size - $disk.FreeSpace) / $disk.Size) * 100)\"");
    printf("-------------------------------------\n");
    log_command("system usage", "SUCCESS");
}

void take_screenshot() {
    printf("-> C Program: Opening Snipping Tool for screenshot...\n");
    system("start explorer ms-screenclip:");
    log_command("take screenshot", "SUCCESS");
}

void open_camera(const char* mode) {
    printf("-> C Program: Opening Camera App...\n");
    system("start microsoft.windows.camera:");
    log_command(mode, "SUCCESS (Camera Opened)");
}

void open_store_and_search(const char* app_name) {
    if (app_name == NULL || strlen(app_name) == 0) return;
    printf("-> C Program: Opening Microsoft Store to search for '%s'...\n", app_name);
    char command[512];
    snprintf(command, sizeof(command), "start ms-windows-store://search/?query=%s", app_name);
    system(command);
    log_command("store search", "SUCCESS");
}

void open_file_explorer() {
    printf("-> C Program: Opening File Explorer...\n");
    system("start explorer.exe");
    log_command("open explorer", "SUCCESS");
}

void show_my_location() {
    printf("-> C Program: Locating your default location (Coimbatore, India) on map...\n");
    char command[512];
    snprintf(command, sizeof(command), "%s \"https://www.google.com/maps/search/?api=1&query=Coimbatore+Tamil+Nadu\"", BROWSER_CMD);
    system(command);
    log_command("show my location", "SUCCESS");
}

void locate_place(const char* place) {
    if (place == NULL || strlen(place) == 0) return;
    printf("-> C Program: Locating '%s' on map...\n", place);
    char* encoded_place = url_encode(place);
    char command[1024];
    snprintf(command, sizeof(command), "%s \"https://www.google.com/maps/search/?api=1&query=%s\"", BROWSER_CMD, encoded_place);
    system(command);
    free(encoded_place);
    log_command("locate place", "SUCCESS");
}

// ==============================================================================
// --- HELPER & EXISTING FUNCTIONS (Some Modified) ---
// ==============================================================================

void show_time() {
    time_t rawtime; struct tm * timeinfo;
    char buffer[80]; time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%I:%M %p", timeinfo);
    printf("\n-> C Program: The current time is %s.\n\n", buffer);
    log_command("get time", "SUCCESS");
}

void open_browser_and_search(const char* base_url, const char* query) {
    char full_url[2048], system_command[2100];
    char* encoded_query = url_encode(query);
    snprintf(full_url, sizeof(full_url), "%s%s", base_url, encoded_query);
    snprintf(system_command, sizeof(system_command), "%s \"%s\"", BROWSER_CMD, full_url);
    printf("-> C Program: Opening browser to search for '%s'\n", query);
    system(system_command);
    log_command(query, "SUCCESS (Browser Opened)");
    free(encoded_query);
}

void log_command(const char *command, const char *status) {
    FILE *file = fopen(LOG_FILE, "a");
    if (file == NULL) return;
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = '\0';
    fprintf(file, "[%s] | Command: %-40s | Status: %s\n", timestamp, command, status);
    fclose(file);
}

void add_task(const char *task_description) {
    if (task_description == NULL || strlen(task_description) == 0) return;
    FILE *file = fopen(TASKS_FILE, "a");
    if (file == NULL) return;
    fprintf(file, "%s\n", task_description);
    fclose(file);
    printf("-> C Program: Task '%s' added successfully.\n", task_description);
    log_command(task_description, "SUCCESS (Added)");
}

void view_tasks() {
    FILE *file = fopen(TASKS_FILE, "r");
    if (file == NULL) {
        printf("\nYour To-Do List is empty.\n\n");
        log_command("view tasks", "SUCCESS (Empty)");
        return;
    }
    printf("\n--- YOUR TO-DO LIST ---\n");
    char line[MAX_LINE_LEN];
    int task_num = 1;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; // Remove newline
        printf("%d. %s\n", task_num++, line);
    }
    printf("-----------------------\n\n");
    fclose(file);
    log_command("view tasks", "SUCCESS");
}

void mark_task_complete(int task_number) {
    FILE *file = fopen(TASKS_FILE, "r");
    if (file == NULL) {
        printf("-> C Program: Task list is empty, nothing to complete.\n");
        return;
    }

    char tasks[MAX_TASKS][MAX_LINE_LEN];
    int count = 0;
    while (count < MAX_TASKS && fgets(tasks[count], sizeof(tasks[0]), file)) {
        tasks[count][strcspn(tasks[count], "\n")] = 0; // Remove newline
        count++;
    }
    fclose(file);

    if (task_number < 1 || task_number > count) {
        printf("-> C Program: Invalid task number.\n");
        return;
    }

    // Prepend [DONE] if not already done
    if (strncmp(tasks[task_number - 1], "[DONE] ", 7) != 0) {
        char updated_task[MAX_LINE_LEN];
        snprintf(updated_task, sizeof(updated_task), "[DONE] %s", tasks[task_number - 1]);
        strcpy(tasks[task_number - 1], updated_task);
    }

    // Rewrite the file
    file = fopen(TASKS_FILE, "w");
    if (file == NULL) {
        printf("-> C Program: Error writing to tasks file.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s\n", tasks[i]);
    }
    fclose(file);

    printf("-> C Program: Task %d marked as complete.\n", task_number);
    log_command("complete task", "SUCCESS");
}


void clear_tasks() {
    if (remove(TASKS_FILE) == 0) {
        printf("\nAll tasks have been cleared.\n\n");
        log_command("clear tasks", "SUCCESS");
    } else {
        printf("\nTask list is already empty or could not be removed.\n\n");
    }
}

char* url_encode(const char* str) {
    char *pstr = (char*)str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
    while (*pstr) {
        if (isalnum(*pstr)) *pbuf++ = *pstr;
        else if (*pstr == ' ') *pbuf++ = '+';
        else *pbuf++ = '%', *pbuf++ = "0123456789ABCDEF"[(*pstr >> 4) & 0xF], *pbuf++ = "0123456789ABCDEF"[*pstr & 0xF];
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}

char* get_substring(const char* source, const char* keyword) {
    const char* keyword_location = strstr(source, keyword);
    if (keyword_location == NULL) return NULL;
    const char* substring_start = keyword_location + strlen(keyword);
    while (*substring_start == ' ') substring_start++;
    char* result = strdup(substring_start);
    return result;
}
