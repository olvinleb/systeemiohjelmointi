#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Define constants for maximum arguments and paths, and buffer size
#define MAX_ARGS 100
#define MAX_PATHS 100
#define BUFFER_SIZE 1024

// Error message to be displayed in case of an error
char error_message[30] = "An error has occurred\n";

// Array to hold the paths
char *paths[MAX_PATHS];
int num_paths = 1; // Number of paths, initialized to 1

/*
    Function declarations so that prototypes can be used below
 */
void execute_command(char **args);
void execute_parallel_commands(char *line);
void process_input(char *line);
void set_path(char **args);
void change_directory(char **args);
void handle_redirection(char **args);
void error();
char *trim_whitespace(char *str);

int main(int argc, char *argv[]) {
    // Initialize paths with default /bin
    paths[0] = strdup("/bin"); // Duplicate the string "/bin" to paths[0]
    paths[1] = NULL; // Null terminate the paths array

    if (argc == 1) {
        // Interactive mode
        char *line = NULL;
        size_t len = 0;
        while (1) {
            printf("wish> "); // Display prompt
            if (getline(&line, &len, stdin) == -1) { // Read a line of input
                break;
            }
            process_input(line); // Process the input line
        }
        free(line); // Free the allocated memory for line
    } else if (argc == 2) {
        // Batch mode
        FILE *file = fopen(argv[1], "r"); // Open the file provided as argument
        if (!file) {
            error(); // Display error if file cannot be opened
            exit(1); // Exit with error code
        }
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, file) != -1) { // Read each line from the file
            process_input(line); // Process each line
        }
        free(line); // Free the allocated memory for line
        fclose(file); // Close the file
    } else {
        error(); // Display error if incorrect number of arguments
        exit(1); // Exit with error code
    }
    return 0; // Return success code
}

void process_input(char *line) {
    // Handle parallel commands
    if (strchr(line, '&') != NULL) { // Check if line contains '&' for parallel commands
        execute_parallel_commands(line); // Execute parallel commands
        return;
    }

    // Tokenize input line into arguments
    char *args[MAX_ARGS];
    int argc = 0;
    char *token = strtok(line, " \t\n"); // Tokenize the line by space, tab, and newline
    while (token != NULL) {
        args[argc++] = token; // Store each token in args array
        token = strtok(NULL, " \t\n"); // Continue tokenizing
    }
    args[argc] = NULL; // Null terminate the args array

    // No command entered
    if (argc == 0) {
        return;
    }

    // Handle built-in commands
    if (strcmp(args[0], "exit") == 0) { // Check if command is "exit"
        if (argc != 1) { // "exit" should not have arguments
            error(); // Display error if incorrect usage
        } else {
            exit(0); // Exit the program
        }
    } else if (strcmp(args[0], "cd") == 0) { // Check if command is "cd"
        if (argc != 2) { // "cd" should have exactly one argument
            error(); // Display error if incorrect usage
        } else {
            change_directory(args); // Change the directory
        }
    } else if (strcmp(args[0], "path") == 0) { // Check if command is "path"
        set_path(args); // Set the paths
    } else {
        handle_redirection(args); // Handle redirection for other commands
    }
}

void execute_command(char **args) {
    // Try executing the command in each path
    for (int i = 0; i < num_paths; ++i) {
        char full_path[BUFFER_SIZE];
        snprintf(full_path, sizeof(full_path), "%s/%s", paths[i], args[0]); // Construct the full path
        if (access(full_path, X_OK) == 0) { // Check if the command is executable
            if (execv(full_path, args) == -1) { // Execute the command
                error(); // Display error if execv fails
                exit(1); // Exit with error code
            }
        }
    }
    error(); // Display error if command not found in any path
    exit(1); // Exit with error code
}

void execute_parallel_commands(char *line) {
    // Split the line into commands using '&' as delimiter
    char *commands[MAX_ARGS];
    int cmd_count = 0;
    char *token = strtok(line, "&"); // Tokenize by '&'
    while (token != NULL) {
        commands[cmd_count++] = trim_whitespace(token); // Trim whitespace and store command
        token = strtok(NULL, "&"); // Continue tokenizing
    }

    pid_t pids[cmd_count];
    for (int i = 0; i < cmd_count; ++i) {
        pids[i] = fork(); // Create a new process
        if (pids[i] == 0) {
            process_input(commands[i]); // Child process handles the command
            exit(0); // Child process exits after handling the command
        } else if (pids[i] < 0) {
            error(); // Display error if fork fails
        }
    }

    for (int i = 0; i < cmd_count; ++i) {
        waitpid(pids[i], NULL, 0); // Parent process waits for all child processes to finish
    }
}

void set_path(char **args) {
    // Free existing paths to prevent memory leaks
    for (int i = 0; i < num_paths; ++i) {
        free(paths[i]);
    }
    num_paths = 0;

    // Copy new paths
    for (int i = 1; args[i] != NULL; ++i) {
        if (num_paths < MAX_PATHS) {
            paths[num_paths++] = strdup(args[i]); // Duplicate the string and store in paths
        } else {
            error(); // Display error if paths exceed maximum limit
            return;
        }
    }
    paths[num_paths] = NULL; // Null terminate the paths array
}

void change_directory(char **args) {
    if (chdir(args[1]) != 0) { // Change directory to the specified path
        error(); // Display error if chdir fails
    }
}

void handle_redirection(char **args) {
    char *cmd[MAX_ARGS]; // Array to hold the command without redirection part
    char *outfile = NULL; // Output file for redirection
    int cmd_idx = 0;

    for (int i = 0; args[i] != NULL; ++i) {
        if (strcmp(args[i], ">") == 0) { // Check for redirection operator
            if (args[i + 1] == NULL || args[i + 2] != NULL || i == 0) { // Validate redirection syntax
                error(); // Display error if syntax is invalid
                return;
            }
            outfile = args[i + 1]; // Set the output file
            break;
        }
        cmd[cmd_idx++] = args[i]; // Store the command part
    }
    cmd[cmd_idx] = NULL; // Null terminate the command array

    pid_t pid = fork(); // Create a new process
    if (pid == 0) {
        if (outfile != NULL) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU); // Open the output file
            if (fd == -1) {
                error(); // Display error if file cannot be opened
                exit(1); // Exit with error code
            }
            dup2(fd, STDOUT_FILENO); // Redirect standard output to file
            dup2(fd, STDERR_FILENO); // Redirect standard error to file
            close(fd); // Close the file descriptor
        }
        execute_command(cmd); // Execute the command
    } else if (pid < 0) {
        error(); // Display error if fork fails
    } else {
        waitpid(pid, NULL, 0); // Parent process waits for the child process to finish
    }
}

void error() {
    write(STDERR_FILENO, error_message, strlen(error_message)); // Write error message to standard error
}

char *trim_whitespace(char *str) {
    while (*str == ' ' || *str == '\t') {
        str++; // Skip leading whitespace
    }
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n')) {
        end--; // Trim trailing whitespace
    }
    *(end + 1) = '\0'; // Null terminate the string
    return str; // Return the trimmed string
}
