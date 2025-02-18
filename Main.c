#include <stdio.h> //holy header files!
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "CommandPrompt.h" // Include the header file for CommandPrompt 

#define MAX_COMMAND_LENGTH 1024 // Maximum length of a command (nobody should really need this much)
#define MAX_ARGS 64 // Maximum number of arguments in a command (nobody should really need this many, either!)

void execute_command(char *command); // Function prototype for execute_command stores a pointer to the command array.
void change_directory(char *path); // Function prototype for change_directory. 
void print_working_directory(); // Function prototype for print_working_directory.

int main() {
    char command[MAX_COMMAND_LENGTH]; // char array that stores the user command input

    while (1) {
        commandPrompt(); // Call the commandPrompt function from CommandPrompt.c

        if (fgets(command, MAX_COMMAND_LENGTH, stdin) == NULL) { //takes the user input and stores it in the command array
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        // Remove newline character from the end of the command, a common issue with fgets
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) { //if the user types exit, the program will break out of the loop and end
            break;
        } else if (strncmp(command, "cd ", 3) == 0) { //if the user types cd, the program will change directories
            change_directory(command + 3);
        } else if (strcmp(command, "pwd") == 0) { //if the user types pwd, the program will print the current working directory
            print_working_directory();
        } else {
            execute_command(command); //if the user types anything else, the program will execute the command
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

    pid_t pid = fork(); //forks the process
    if (pid == -1) {  //if the fork fails, the program will print an error message and exit
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  //if the fork is successful, the child process will execute the command
        if (execvp(args[0], args) == -1) { //execvp() executes the command stored in the args array
            perror("execvp failed");
        }
        exit(EXIT_FAILURE);
    } else {
        wait(NULL); //the parent process will wait for the child process to finish
    }
}

void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("chdir failed");
    }
}

void print_working_directory() {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL) { //getcwd() gets the current working directory and stores it in the cwd array
        printf("%s\n", cwd);
    } else {
        perror("getcwd() error");
    }
}
