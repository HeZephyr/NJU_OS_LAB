#include <stdio.h>
#include <unistd.h>

int main() {
    pid_t pid1 = fork();
    pid_t pid2 = fork();
    pid_t pid3 = fork();
    pid_t pid  = getpid();
    printf("The PID of the current process is %d\n Hello World from (%d, %d, %d)\n", pid, pid1, pid2, pid3);
    return 0;
}
