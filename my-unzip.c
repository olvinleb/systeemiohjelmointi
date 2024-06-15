#include <stdio.h> 
#include <stdlib.h>

// Function to decompress the RLE encoded data
void decompress(FILE *stream) {
    int run_length;
    char character;

    // Read the run length and character in 5-byte chunks
    while (fread(&run_length, sizeof(int), 1, stream) == 1) {
        if (fread(&character, sizeof(char), 1, stream) != 1) {
            fprintf(stderr, "wunzip: file format error\n");
            exit(1);
        }
        // Print the character run_length times
        for (int i = 0; i < run_length; i++) {
            fputc(character, stdout);
        }
    }
}

int main(int argc, const char *argv[]) {
    // Check if there are enough arguments
    if (argc < 2) {
        fprintf(stdout, "wunzip: file1 [file2 ...]\n");
        exit(1);
    }

    // Process each file provided as an argument
    for (int i = 1; i < argc; ++i) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "wunzip: cannot open file %s\n", argv[i]);
            exit(1);
        }
        decompress(fp);
        fclose(fp);
    }

    return 0;
}