#include <stdio.h> //holy header files!
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
// #include "CommandPrompt.h" // Include the header file for CommandPrompt - taken out and added to the bottom of the file

//Definitions
#define MAX_COMMAND_LENGTH 1024 // Maximum length of a command (nobody should really need this much)
#define MAX_ARGS 64 // Maximum number of arguments in a command (nobody should really need this many, either!)

// function prototypes
void execute_command(char *command); // Function prototype for execute_command stores a pointer to the command array.
void change_directory(char *path); // Function prototype for change_directory. 
void print_working_directory(); // Function prototype for print_working_directory.
void commandPrompt(); // Function prototype for commandPrompt.

// Main function
int main() {
    char command[MAX_COMMAND_LENGTH]; // char array that stores the user command input

    while (1) {
        commandPrompt(); // Call the commandPrompt function from CommandPrompt.h

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
    int background = 0;
    char *input_file = NULL;
    char *output_file = NULL;


    token = strtok(command, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        if (strcmp(token, "<") == 0) { // Input redirection
            token = strtok(NULL, " ");
            if (token) input_file = token;
        } else if (strcmp(token, ">") == 0) { // Output redirection
            token = strtok(NULL, " ");
            if (token) output_file = token;
        } else if (strcmp(token, "&") == 0) { // Background execution
            background = 1;
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    if (args[0] == NULL) { //if the user doesn't enter a command, the program will return   
        return;
    }

    pid_t pid = fork(); //fork() creates a new process by duplicating the calling process. The new process is the child, and the calling process is the parent process.
    if (pid == -1) { //if fork() fails, the program will print an error message and exit
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        // Handle input redirection
        if (input_file) { //if the user enters a file to read from, the program will open the file and read from it 
            int fd = open(input_file, O_RDONLY);
            if (fd == -1) {
                perror("Error opening input file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO); //dup2() duplicates an open file descriptor. In this case, it duplicates the file descriptor for the input file
            close(fd);
        }

        // Handle output redirection
        if (output_file) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); //open() opens a file and returns a file descriptor
            if (fd == -1) {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd); //close() closes the file descriptor 
        }

        if (execvp(args[0], args) == -1) {
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
    } else { // Parent process
        if (!background) { //if the user doesn't enter an &, the program will wait for the child process to finish
            waitpid(pid, NULL, 0); // Wait if not running in background
        } else {
            printf("Process running in background (PID: %d)\n", pid);
        }
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

void commandPrompt() {
    char cwd[PATH_MAX]; // PATH_MAX is a constant defined in linux/limits.h that represents the maximum length of a pathname.

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s $ ", cwd);    //print the current working directory followed by a $ sign
        fflush(stdout); //ensures that the prompt is printed to the screen immediately
    } else {
        perror("getcwd() error"); //prints an error message if getcwd() fails
        //this is helpful for debugging in case something breaks
    }
}