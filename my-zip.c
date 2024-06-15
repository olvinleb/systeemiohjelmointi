#include <stdio.h> 
#include <stdlib.h>

// Struct to hold run-length encoding (RLE) information
struct token {
    int l;   // Length of the run
    char c;  // Character in the run
};

// Function to write an RLE object to standard output
void write_rle(struct token rleobj) {
    fwrite(&(rleobj.l), sizeof(int), 1, stdout); // Write the run length
    fwrite(&(rleobj.c), sizeof(char), 1, stdout); // Write the character
}

// Function to process a file and update the RLE object
struct token process(FILE *stream, struct token prev) {
    int curr;
    struct token rle;

    // Read characters from the file
    while ((curr = fgetc(stream)) != EOF) {
        if (prev.c != '\0' && curr != prev.c) { // If the character changes
            rle.c = prev.c;
            rle.l = prev.l;
            prev.l = 0;
            write_rle(rle); // Write the previous RLE object
        }
        prev.l++; // Increment the run length
        prev.c = curr; // Update the character
    }
    rle.c = prev.c;
    rle.l = prev.l;
    return rle;
}

int main(int argc, const char *argv[]) {
    // Check if there are enough arguments
    if (argc < 2) {
        fprintf(stdout, "wzip: file1 [file2 ...]\n");
        exit(1);
    }

    struct token prev;
    prev.c = '\0';
    prev.l = 0;

    // Process each file provided as an argument
    for (int i = 1; i < argc; ++i) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stdout, "wzip: cannot open file %s\n", argv[i]);
            exit(1);
        }
        struct token rle = process(fp, prev);
        prev.c = rle.c;
        prev.l = rle.l;
        fclose(fp);
    }

    // Write the last RLE object
    write_rle(prev);
    return 0;
}