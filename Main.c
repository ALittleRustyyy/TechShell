//Devin Walton
//Desc: This program takes in user input and then separates it into different words(tokens) before returning them to the user
//01/28/2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //for fork(), execvp(), dup2(), close(), etc
#include <fcntl.h> //for open(), 0_RDONLY, 0_WRONLY, 0_CREAT, 0_TRUNC, 0_APPEND, etc
#include <sys/wait.h> //for waitpid
#include <sys/types.h> //for pid_t


#define MAX_TOKEN_SIZE 256

// Function Prototypes
void tokenize(char *str); //depricated
void ExecuteCommand(char *commandArgs[]);

int main() {
    while(1){
        printf("$ ");
        char user_in[MAX_TOKEN_SIZE];
        //fgets(user_in, MAX_TOKEN_SIZE, stdin);

        fflush(stdout);  // Ensure prompt is displayed immediately

        // Read user input
        if (fgets(user_in, MAX_TOKEN_SIZE, stdin) == NULL) { //if user input is null
            break;  // Stop
        }

        // Remove newline character from input
        size_t len = strlen(user_in);
        if (len > 0 && user_in[len - 1] == '\n') {
            user_in[len - 1] = '\0'; //replace it with null
        }

        // Check if user entered "exit"
        if (strcmp(user_in, "exit") == 0) {
            break;  // Stop execution
        }

        // Print the line read
        printf("Line read: %s\n", user_in);

        // Tokenize input
        printf("Token(s):\n");
        int count = 0;
        char *token = strtok(user_in, " ");  // Get first token

        while (token != NULL) {
            printf(" %s\n", token);  // Print token
            count++;
            token = strtok(NULL, " ");  // Get next token (strtok remembers where I was last)
        }

        // Print token count
        printf("%d token(s) read\n\n", count);
    }
    return 0;
}

void ExecuteCommand(char *commandArgs[]){ 
    pid_t pid = fork();  // Create a new process

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");  // Print error message
        exit(1);  // Exit with error code
    } else if (pid == 0) { // Child process
        // Handle I/O redirection
        HandleIO(commandArgs); // Call HandleIO function to handle I/O redirection @devi-walto come back to this

        // Execute command
        if(execvp(commandArgs[0], commandArgs) == -1) {  // Replace child process with new program
            perror("Execution failed");  // Print error message if execvp fails
            exit(1);  // Exit with error code
        }
    } else { // Parent process
        // Parent process waits for child to complete
        waitpid(pid, NULL, 0);  // Wait for child process to finish
    }
}

void HandleIO(char *commandArgs[]){
    int i = 0; // Initialize index
    int inputfd = -1; // Initialize input file descriptor
    int outputfd = -1; // Initialize output file descriptor

    while (commandArgs[i] != NULL) { // Loop through command arguments
        //INPUT REDIRECTION
        if (strcmp(commandArgs[i], "<") == 0) { // Check for input redirection
            inputfd = open(commandArgs[i + 1], O_RDONLY); // Open input file 0_RDONLY - read only
            if (inputfd < 0) { // Check if open failed
                perror("Input redirection failed"); // Print error message
                exit(1); // Exit with error code
            }
            dup2(inputfd, STDIN_FILENO); // Duplicate input file descriptor to stdin STDIN_FILENO - standard input
            close(inputfd); // Close input file descriptor
            commandArgs[i] = NULL; // Remove input redirection from command arguments
        }
        //OUTPUT REDIRECTION
        else if (strcmp(commandArgs[i], ">") == 0) { // Check for output redirection
            outputfd = open(commandArgs[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open output file 0_WRONLY - write only, 0_CREAT - create file if it doesn't exist, 0_TRUNC - truncate file to 0 length
            if (outputfd < 0) { // Check if open failed
                perror("Output redirection failed"); // Print error message
                exit(1); // Exit with error code
            }
            dup2(outputfd, STDOUT_FILENO); // Duplicate output file descriptor to stdout
            close(outputfd); // Close output file descriptor
            commandArgs[i] = NULL; // Remove output redirection from command arguments
        }
        // OUTPUT REDIRECTION WITH APPEND
        else if (strcmp(commandArgs[i], ">>") == 0) { // Check for output redirection
            outputfd = open(commandArgs[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644); // Open output file 0_WRONLY - write only, 0_CREAT - create file if it doesn't exist, 0_APPEND - append to file
            if (outputfd < 0) { // Check if open failed
                perror("Output redirection failed"); // Print error message
                exit(1); // Exit with error code
            }
            dup2(outputfd, STDOUT_FILENO); // Duplicate output file descriptor to stdout STDOUT_FILENO - standard output
            close(outputfd); // Close output file descriptor
            commandArgs[i] = NULL; // Remove output redirection from command arguments
        }
        i++; // Increment index
    }
}