#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>

void commandPrompt() {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s $", cwd);
        fflush(stdout);
    } else {
        perror("getcwd() error");
    }
}