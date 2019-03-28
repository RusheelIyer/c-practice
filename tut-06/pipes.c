#include <stdlib.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char const *argv[])
{
    int pid, pipefd[2];

    // Create the pipe. Parent has write and read end open
    pipe(pipefd);

    pid = fork();
    if (pid == 0) {
        dup2(pipefd[WRITE_END], STDOUT_FILENO);

        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]);

        execlp("/bin/ls", "ls", NULL);
    } else {

        pid = fork();
        if (pid == 0) {
            dup2(pipefd[WRITE_END], STDOUT_FILENO);

            close(pipefd[READ_END]);
            close(pipefd[WRITE_END]);

            execlp("/bin/less", "less", NULL);

        } else {
            close(pipefd[READ_END]);
            close(pipefd[WRITE_END]);        

            wait(NULL);
            wait(NULL);
        }
    }
}
