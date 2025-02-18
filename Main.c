//Devin Walton
//Desc: This program takes in user input and then separates it into different words(tokens) before returning them to the user
//01/28/2025

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        if(execvp(commandArgs[0], commandArgs) == -1) {  // Replace child process with new program
            perror("Execution failed");  // Print error message if execvp fails
            exit(1);  // Exit with error code
        }
    } else { // Parent process
        // Parent process waits for child to complete
        waitpid(pid, NULL, 0);  // Wait for child process to finish
    }
}