#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Function to check that two files are lonked
int filesarelinked(const char *file1, const char *file2) {
    // Declare two variables of struct stat to help us retrieve file status
    struct stat stat1, stat2;

    // Get file status for the first file
    if (stat(file1, &stat1) != 0) {
        perror("Error getting file status for file1");
        return 1;
    }

    // Get file status for the second file
    if (stat(file2, &stat2) != 0) {
        return 1;
    }

    // Compare inode number and device ID to tell if
    if (stat1.st_ino == stat2.st_ino && stat1.st_dev == stat2.st_dev) {
        return 1; // Files are linked
    } else {
        return 0; // Files are not linked
    }
}

// Function to read inpit from a provided file and then write it to another provided file
void readfilewritefile(const char *filenamer, const char *filenamew) {
    // Define a line that will be read as null first
    char *line = NULL;
    // Keep the size
        size_t len = 0;
        int numberoflines = 0;

        FILE *fr = fopen(filenamer, "r");
        FILE *fw = fopen(filenamew, "w");

        // Checks for succesfull file openings
        if (!fr) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", filenamer);
            exit(1);
        }
        if (!fw) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", filenamew);
            exit(1);
        }

        // Check for files to not be the same (either the same name or acrually pointing to the same file in case of linking)
        if ((strcmp(filenamer, filenamew) == 0) || (filesarelinked(filenamer, filenamew) != 0)) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            exit(1);
            }

        // Count the number of lines in the input file
        // Will be used later to go backwards and write lines to output
        while (getline(&line, &len, fr) != -1) {
            numberoflines++;
        }
        rewind(fr); // Reset file pointer to the beginning of the file

        //
        char **linestowrite = malloc(numberoflines * sizeof(char *));
        if (linestowrite == NULL) {
            fprintf(stderr, "malloc failed");
            exit(1);
        }

        // Read lines from the file and store them in the array
        for (int i = 0; i < numberoflines; i++) {
            if (getline(&line, &len, fr) == -1) {
                perror("Error reading line");
                exit(1);
            }
            // Allocate memory for the line and copy its contents
            linestowrite[i] = malloc(strlen(line) + 1);
            if (linestowrite[i] == NULL) {
                fprintf(stderr, "malloc failed");
                exit(1);
            }
            strcpy(linestowrite[i], line);
        }

        // Write lines to the output file in reverse order
        for (int i = numberoflines - 1; i >= 0; i--) {
            fprintf(fw, "%s", linestowrite[i]);
            free(linestowrite[i]); // Free memory allocated for each line
        }
        free(linestowrite); // Free memory allocated for the array

        // Close files
        fclose(fr);
        fclose(fw);
        if (line) free(line); // Free memory allocated by getline
    }

    // Function to write reverse text to stdout from a provided file
    void readfilewriteoutput(const char *filenamer) {
        // The logic below is identical from a function above
        // Except for writing to stdout
        char *line = NULL;
        size_t len = 0;
        int numberoflines = 0;
        FILE *fr = fopen(filenamer, "r");

        if (!fr) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", filenamer);
            exit(1);
        }

        // Count the number of lines in the input file
        while (getline(&line, &len, fr) != -1) {
            numberoflines++;
        }
        rewind(fr); // Reset file pointer to the beginning of the file

        char **linestowrite = malloc(numberoflines * sizeof(char *));
        if (linestowrite == NULL) {
            fprintf(stderr, "malloc failed");
            exit(1);
        }

        // Read lines from the file and store them in the array
        for (int i = 0; i < numberoflines; i++) {
            if (getline(&line, &len, fr) == -1) {
                perror("Error reading line");
                exit(1);
            }
            // Allocate memory for the line and copy its contents
            linestowrite[i] = malloc(strlen(line) + 1);
            if (linestowrite[i] == NULL) {
                fprintf(stderr, "malloc failed");
                exit(1);
            }
            strcpy(linestowrite[i], line);
        }

        // Write lines to the output file in reverse order
        for (int i = numberoflines - 1; i >= 0; i--) {
            fprintf(stdout, "%s", linestowrite[i]);
            free(linestowrite[i]); // Free memory allocated for each line
        }
        free(linestowrite); // Free memory allocated for the array

        // Close files
        fclose(fr);
        if (line) free(line); // Free memory allocated by getline
    }

    // Function to read from stdin and write to stdout
    // Same as above but uses stdin and stdout for input and output
    void readstdinwritestdout() {
        char *line = NULL;
        size_t len = 0;
        int numberoflines = 0;
        FILE *fr = stdin;

        // Count the number of lines in the input
        while (getline(&line, &len, fr) != -1) {
            numberoflines++;
        }
        rewind(fr); // Reset file pointer to the beginning of the stdin (doesn't work for actual stdin, but for the sake of example)

        char **linestowrite = malloc(numberoflines * sizeof(char *));
        if (linestowrite == NULL) {
            fprintf(stderr, "malloc failed");
            exit(1);
        }

        // Read lines from stdin and store them in the array
        for (int i = 0; i < numberoflines; i++) {
            if (getline(&line, &len, fr) == -1) {
                perror("Error reading line");
                exit(1);
            }
            // Allocate memory for the line and copy its contents
            linestowrite[i] = malloc(strlen(line) + 1);
            if (linestowrite[i] == NULL) {
                fprintf(stderr, "malloc failed");
                exit(1);
            }
            strcpy(linestowrite[i], line);
        }

        // Write lines to stdout in reverse order
        for (int i = numberoflines - 1; i >= 0; i--) {
            fprintf(stdout, "%s", linestowrite[i]);
            free(linestowrite[i]); // Free memory allocated for each line
        }
        free(linestowrite); // Free memory allocated for the array

        if (line) free(line); // Free memory allocated by getline
    }

    // Parse arguments and determine what function to call
    // Breaks are necessary for
    int main(int argc, char *argv[]) {
        switch (argc) {
            case 1:
                readstdinwritestdout();
                break;
            case 2:
                readfilewriteoutput(argv[1]);
                break;
            case 3:
                readfilewritefile(argv[1], argv[2]);
                break;
            default:
                fprintf(stderr, "usage: reverse <input> <output>\n");
                exit(1);
        }
        return 0;
    }
    