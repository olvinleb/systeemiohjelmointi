#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Function to read all given files and return a code to be used in main
int readfiles(char **files, int num_files) {
    // Iterate though all provided files
    for (int i = 0; i < num_files; i++) {
        char *line = NULL;
        size_t len = 0;
        FILE *fr = fopen(files[i], "r");

        if (!fr) {
            fprintf(stdout, "wcat: cannot open file\n");
            return 1;  // Fail now as per requirements
        }

        // Print each line in the file to stdout
        while (getline(&line, &len, fr) != -1) {
            fprintf(stdout, "%s", line);
        }

        // Close the file and free the allocated line
        fclose(fr);
        if (line) {
            free(line);
            line = NULL;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // Exit gracefully if no arguments are provided
        exit(0);
    }

    // Execute the function from above and get a return code
    int mycatreutrn = readfiles(&argv[1], argc - 1);

    // Return from main whatever was returned by call to read files
    return mycatreutrn;
}