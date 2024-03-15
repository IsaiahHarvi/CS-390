// Author: Isaiah Harville
// Date:   2/10/2024
// To Compile: gcc -ansi -o mysh harville_mysh.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_INPUT_SIZE 512
#define MAX_ARGS 20

// Default Prompt
char prompt[50] = "$ ";

// Function Declarations
void execute_command(char **args);
void echo_wrapper(char **args);
void change_prompt_style(char *newPrompt);
void cat_wrapper(char **args);
void rm_wrapper(char **args);
void cp_wrapper(char **args);
void mkdir_wrapper(char **args);
void rmdir_wrapper(char **args);
void ls_wrapper(char **args);
void cd_wrapper(char **args);
void display_help();
void exit_wrapper();

// Command Strcutre
typedef struct {
    char *name; // Command name
    union {
        void (*mp_func)(char **args); // multi-parameter function
        void (*sp_func)(char *arg);   // single-parameter function
        void (*np_func)();            // no-parameter function
    } instruction;
    int params; // 2 **args, 1 *arg, 0 no args
} Command;

// List of commands
Command commands[] = {
    // {Command Name, {Function}, Number of Parameters}
    {"echo", {.mp_func = echo_wrapper}, 2}, 
    {"cd", {.mp_func = cd_wrapper}, 2}, 
    {"cat", {.mp_func = cat_wrapper}, 2}, 
    {"rm", {.mp_func = rm_wrapper}, 2}, 
    {"cp", {.mp_func = cp_wrapper}, 2}, 
    {"mkdir", {.mp_func = mkdir_wrapper}, 2},
    {"rmdir", {.mp_func = rmdir_wrapper}, 2},
    {"ls", {.mp_func = ls_wrapper}, 2},
    {"PS1", {.sp_func = change_prompt_style}, 1},
    {"help", {.np_func = display_help}, 0},
    {"exit", {.np_func = exit_wrapper}, 0},
    {NULL, {NULL}, 0} // End of list
};

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    char *token;

    while (1 == 1) { // Infinite loop
        printf("%s", prompt); // Print the prompt
        fgets(input, MAX_INPUT_SIZE, stdin); // Get the input from the user

        size_t length = strlen(input); // Get the length of the input
        if (input[length - 1] == '\n') { // Remove the newline character
            input[length - 1] = '\0'; // Replace the newline character with a null terminator
        }

        // Parse the input
        int i = 0;
        token = strtok(input, " "); // Split the input into tokens
        while (token != NULL) { 
            args[i++] = token; // Add the token to the args array
            token = strtok(NULL, " ");
        } 
        args[i] = NULL; // Add a null terminator to the end of the args array

        // Execution
        execute_command(args);
    }
}

void execute_command(char **args) {
    int i = 0;
    while (commands[i].name != NULL) {
        if (strcmp(commands[i].name, args[0]) == 0) {
            if (commands[i].params == 2) {
                // Execute multi-parameter function
                commands[i].instruction.mp_func(args);
            } else if (commands[i].params == 1) {
                // Execute single parameter function
                commands[i].instruction.sp_func(args[1]);
            } else {
                // Execute no parameter function
                commands[i].instruction.np_func();
            }
            return;
        }
        i++; // Move to the next command
    }
    printf("Command Unsupported\n");
}

void echo_wrapper(char **args) {
    if (args[1] == NULL){ // If empty argument
        printf("\n"); // Print newline if no arguments
    } else { // If not empty argument
        int i = 1;
        if (strcmp(args[1], "-n") == 0) i = 2; // Skip the new line if theres a -n
        while (args[i] != NULL) {
            printf("%s ", args[i++]); // Print the arguments
        }
        if (strcmp(args[1], "-n") != 0) printf("\n"); // Print newline unless -n
    }
}

void change_prompt_style(char *newPrompt) {
    strcpy(prompt, newPrompt); // Copy the new prompt to the prompt variable
    strcat(prompt, " "); // Add a space after the new prompt for readabiloty
}

void cat_wrapper(char **args) {
    FILE *file;  // File pointer
    char line[MAX_INPUT_SIZE];  // Line buffer

    // Open the file for reading
    if ((file = fopen(args[1], "r")) == NULL) { 
        perror("Error opening file !");
        return;
    }

    // Print the contents x`of the file to the terminal
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }
    fclose(file); // Close the file
}

void rm_wrapper(char **args) {
    if (remove(args[1]) != 0) {
        perror("Error deleting file !");
    }
}

void cp_wrapper(char **args) {
    FILE *source, *destination;
    int ch;
    // Check for the correct number of arguments
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "Error !\nUsage: cp <source> <destination>\n"); // Print error/usage msg
        return;
    }

    // Open the source file for reading
    source = fopen(args[1], "rb");
    if (source == NULL) {
        perror("Error opening source file");
        return;
    }

    // Open the destination file for writing
    destination = fopen(args[2], "wb");
    if (destination == NULL) {
        perror("Error opening destination file");
        fclose(source);
        return;
    }

    // Copy the contents of the source file to the destination file
    while ((ch = fgetc(source)) != EOF) {
        fputc(ch, destination); // Write the character to the destination file
    }

    // Close both files
    fclose(source);
    fclose(destination);
}

void mkdir_wrapper(char **args) {
    if (mkdir(args[1], 0777) != 0) { // we dont care about security, freedom for all files
        perror("Error creating directory !");
    }
}

void rmdir_wrapper(char **args) {
    if (rmdir(args[1]) != 0) {
        perror("Error deleting directory !");
    }
}

void ls_wrapper(char **args) {
    char command[256] = "ls "; // Create the ls command
    int i = 1; // Start at the first argument
    while (args[i] != NULL) {
        strcat(command, args[i]); // Add the argument to the command
        strcat(command, " ");  // Add space between arguments
        i++; // Move to the next argument
    }
    system(command);  // Execute the ls command
}

void cd_wrapper(char **args) {
    if (args[1] == NULL) { // empty argument
        fprintf(stderr, "Expected argument to \"cd\"\n"); // print error msg/usage
    } else {
        if (chdir(args[1]) != 0) { // change directory
            perror("cd failed"); // tell user if it failed
        } else {
            change_prompt_style(strcat(args[1], " $")); // include new dir in prompt
        }
    }
}

void display_help() { // Display the help message
    printf("\techo  [optional -n] [args..] - prints the arguments to the terminal with an option to not include a newline\n");
    printf("\tPS1   [new_character]        - changes the prompt msg to the specified chars\n");
    printf("\tcat   [filename]             - prints the contents of the file to the terminal\n");
    printf("\tcp    [source] [destination] - copies the source file to the destination file\n");
    printf("\trm    [filename]             - deletes the file\n");
    printf("\tmkdir [dirname]              - creates a new directory\n");
    printf("\trmdir [dirname]              - deletes the directory\n");
    printf("\tls    [optional -l -a..]     - lists the contents of the current directory\n");
    printf("\tcd    [dirname]              - changes the current directory to the specified directory and updates the prefix\n");
    printf("\texit                         - exits this shell\n");
    printf("\thelp                         - displays this help message\n");
}

void exit_wrapper() {
    printf("Goodbye!\n");
    exit(0);
}