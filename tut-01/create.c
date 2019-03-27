#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{

    pid_t pid = fork();
    switch (pid) {
        case -1:
            printf("Error. fork failed\n");
            break;
        case 0:
            printf("I am the child.\n");
            break;
        default:
            printf("I am the parent.\n");
            wait(NULL);
            printf("Child terminated.\n");
    }

    return 0;
}
