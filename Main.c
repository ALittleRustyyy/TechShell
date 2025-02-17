#include <stdio.h> //holy header files!
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "CommandPrompt.c" // Include the header file for CommandPrompt

#define MAX_COMMAND_LENGTH 1024 // Maximum length of a command (nobody should really need this much)
#define MAX_ARGS 64 // Maximum number of arguments in a command (nobody should really need this many, either!)

void execute_command(char *command); // Function prototype for execute_command stores a pointer to the command array.
void change_directory(char *path); // Function prototype for change_directory. 

int main() {
    char command[MAX_COMMAND_LENGTH]; // char array that stores the user command input

    while (1) {
        commandPrompt(); // Call the commandPrompt function from CommandPrompt.c

        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        // Remove newline character from the end of the command
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strncmp(command, "cd ", 3) == 0) {
            change_directory(command + 3);
        } else if (strcmp(command, "pwd") == 0) {
            print_working_directory();
        } else {
            execute_command(command);
        }
    }

    return 0;
}

void execute_command(char *command) {
    char *args[MAX_ARGS];
    char *token;
    int i = 0;

    token = strtok(command, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("chdir failed");
    }
}

//Devin, this is a lot of empty functionality im working on, feel free to add to it or make a 
//new branch to work on your own stuff. I'm thinking that we can import various methods from 
//different files to make this work, so i may end up pushing the methods in this main file to
//their own individual files.