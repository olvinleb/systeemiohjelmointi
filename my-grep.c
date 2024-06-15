#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to perfrom search in a file provided
void search_in_file(FILE *file, const char *search_term) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // While there are lines to read (no carriage character) read the file line by line
    while ((read = getline(&line, &len, file)) != -1) {
        // Get a first occurance of a sought after string
        if (strstr(line, search_term) != NULL) {
            // Print it to stdout
            fprintf(stdout, "%s", line);
        }
    }
    // Deallocate memeory for lines if there are any
    free(line);
}

int main(int argc, char *argv[]) {
    // If no arguments are provided
    // Exit with an error
    if (argc < 2) {
        fprintf(stdout, "wgrep: searchterm [file ...]\n");
        exit(1);
    }

    // Get a sought after string
    const char *search_term = argv[1];

    if (argc == 2) {
        // No files specified, read from stdin
        search_in_file(stdin, search_term);
    } else {
        // Files specified, process each file separatley
        for (int i = 2; i < argc; i++) {
            FILE *file = fopen(argv[i], "r");
            if (!file) {
                fprintf(stdout, "wgrep: cannot open file\n");
                exit(1);
            }
            search_in_file(file, search_term);
            fclose(file);
        }
    }

    return 0;
}