/* Author: Isaiah Harville 
/* Date:   4/3/2024
/* To Compile: gcc -o filter Harville_cs390program3.c  
/* To run test cases: ./unittest.sh
/* To use standalone: echo [filename] | ./filter [-u: optional] */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ONE_MB 1000000

unsigned char* buffer;
int bufferSize = 5 * ONE_MB; // 5 MB buffer size

int read_binf(const char* filename);
int write_binf(const char* filename, const char* new_filename);
int text_type(unsigned char* buffer, int dataSize);
int to_unix(unsigned char* buffer, int* dataSize);

int main(int argc, char* argv[]) {
    char filename[256];
    int convert = 0; // bool value

    // Check if '-u' argument is provided
    if (argc > 1 && strcmp(argv[1], "-u") == 0) {
        convert = 1;
    }

    buffer = (unsigned char*)malloc(bufferSize);
    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }

    while (fgets(filename, sizeof(filename), stdin)) {
        // Remove newline character
        filename[strcspn(filename, "\n")] = '\0';
        int status = read_binf(filename);

        if (status > 0) {
            switch (status) {
                case 1: fprintf(stderr, "The file could not be opened\n"); break;
                case 2: fprintf(stderr, "It is a zero-length file\n"); break;
                case 3: fprintf(stderr, "File too big for buffer\n"); break;
                case 4: fprintf(stderr, "There was an error reading the file\n"); break;
            }
        } 
        else {
            int data_size = strlen((char*)buffer); // Assuming the buffer was null-terminated by read_binf
            int state = text_type(buffer, data_size);
            if (state == 0) {
                fprintf(stderr, "File %s is NOT text\n", filename);
            } 
            else if (state == 1) {
                fprintf(stdout,"File is Windows Compliant\n");
                if (convert) { to_unix(buffer, &data_size); }
            }
            else if (state == 2) {
                fprintf(stdout,"File is Unix Compliant\n");
            }
        }
    }

    free(buffer);
    return 0;
}

int read_binf(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return 1; // File could not be opened
    }

    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (fileSize == 0) {
        fclose(file);
        return 2; // Zero length file
    }
    if (fileSize >= bufferSize) {
        fclose(file);
        return 3; // File too big for buffer
    }

    int bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
        fclose(file);
        return 4; // Error reading file
    }

    buffer[bytesRead] = '\0';
    fclose(file);
    return 0;
}

int text_type(unsigned char* buffer, int dataSize) {
    int cr = 0; // Flag to indicate carriage return '\r'
    
    for (int i = 0; i < dataSize; ++i) {
        if (buffer[i] == '\r') { cr = 1; }
        if (!isprint(buffer[i]) && buffer[i] != '\n' && buffer[i] != '\r' && buffer[i] != '\t' && buffer[i] != '\f' && buffer[i] != '\0') {
            return 0; // Not a text file
        }
    }

    if (cr) { return 1; } // windows
    return 2; // unix
}

int to_unix(unsigned char* buffer, int* dataSize) {
    int j = 0;
    for (int i = 0; i < *dataSize; ++i) {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n') {
            continue; // Skip carriage return
        }
        buffer[j++] = buffer[i];
    }
    *dataSize = j;
    buffer[j] = '\0'; // Null-terminate the modified buffer
    return 1;
}

int write_binf(const char* filename, const char* new_filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return 0; // Failed to open file for writing
    }

    int dataSize = strlen((char*)buffer);
    fwrite(buffer, 1, dataSize, file);
    fclose(file);

    if (rename(filename, new_filename)) { return 2; } // Failed to rename

    return 1; // Success
}