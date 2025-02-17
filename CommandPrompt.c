#include <stdio.h> //standard includes for IO and function prototypes. 
#include <unistd.h> //(UNISTD.H is for interacting with the OS) also includes the getcwd() function
#include <linux/limits.h> //allows for the PATH_MAX constant to be referenced

void commandPrompt() {
    char cwd[PATH_MAX]; // PATH_MAX is a constant defined in linux/limits.h that represents the maximum length of a pathname.

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s $", cwd);    //print the current working directory followed by a $ sign
        fflush(stdout); //ensures that the prompt is printed to the screen immediately
    } else {
        perror("getcwd() error"); //prints an error message if getcwd() fails
        //this is helpful for debugging in case something breaks
    }
}