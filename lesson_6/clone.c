#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <sys/wait.h>

int thread_func(void *arg) {
    printf("Child thread started\n");
    return 0;
}

int main(int argc, char *argv[]) {
    // 为子线程分配堆栈空间
    void *stack = malloc(1024*1024);
    assert(stack);

    pid_t pid = clone(thread_func, stack + 1024 + 1024, CLONE_VM, NULL);
    if (pid < 0) {
        perror("clone error");
        free(stack);
        exit(EXIT_FAILURE);
    }

    printf("Parent thread waiting for child...\n");
    waitpid(pid, NULL, 0);
    printf("Child thread finished\n");
    free(stack);
    return 0;
}
