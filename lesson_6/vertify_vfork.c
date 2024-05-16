#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>

int main() {
    // 在父进程中分配内存并初始化
    char *buffer = malloc(1024);
    assert(buffer != NULL);
    memset(buffer, 'A', 1024);

    // 使用vfork创建子进程
    pid_t pid = vfork();
    if (pid < 0) {
        perror("vfork error");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // 子进程
        printf("Child process: PID = %d\n", getpid());
        
        // 修改内容
        memset(buffer, 'B', 1024);
        // 子进程
        char * const argv[] = {"/bin/echo", "Hello, Linux!", NULL};
        char * const envp[] = {NULL};
        // 执行exec函数
        execve(argv[0], argv, envp);
    } else {
        // 父进程
        printf("Parent process: PID = %d, child's PID = %d\n", getpid(), pid);
        // 验证内存内容是否被子进程修改
        for (int i = 0; i < 1024; i++) {
            if (buffer[i] != 'B') {
                printf("Memory corruption detected at index %d\n", i);
                exit(EXIT_FAILURE);
            }
        }
        printf("Memory is consistent\n");
    }
    return 0;
}
