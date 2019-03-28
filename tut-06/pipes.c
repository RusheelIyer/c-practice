#include <stdlib.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int main(int argc, char const *argv[])
{
    int pid, pipefd[2];

    pipe(pipefd);
}
