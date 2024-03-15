#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

void ls_wrapper(char **args) {
    char command[256] = "ls "; /* Create the ls command  */
    int i = 1; /* Start at the first argument  */
    while (args[i] != NULL) {
        strcat(command, args[i]); /* Add the argument to the command  */
        strcat(command, " ");  /* Add space between arguments  */
        i++; /* Move to the next argument  */
    system(command);  /* Execute the ls command  */
    }
}

int main(int argc, char *argv[]) {
    /* Pass the command-line arguments to ls_wrapper, skipping the program name  */
    ls_wrapper(argv);
    return 0;
}