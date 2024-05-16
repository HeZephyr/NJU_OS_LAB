#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024) /* Stack size for cloned child */
// 宏，简化错误处理
#define ERREXIT(msg) { perror(msg); exit(EXIT_FAILURE); }
// 安全分配内存函数，分配失败报告错误
#define CHECKALLOC(ptr, msg)  ({ void *p = ptr; if (p == NULL) {ERREXIT(msg);} p;})

/*
 * 子进程函数
 * params: 接受一个void *类型参数，但是没有被使用过，后面的声明是用于告诉编译器这个参数是未被使用的
 */
static int childFunc(void *arg __attribute__((unused))) {
    puts("child: start");
    sleep(2);
    puts("child: terminate");
    return 0; /* Child terminates now */

}

int main(int argc, char *argv[]) {
    /* Start of stack buffer */
    char **stacks; 
    /* Child process's pids */
    pid_t *pids;   
    size_t nproc, i;

    // 接受两个参数
    if (argc != 2) {
        puts("Wrong way to execute the program:\n"
                "\t\t./waitpid nProcesses\n"
                "example:\t./waitpid 2");

        return EXIT_FAILURE;

    }
    // 初始化nproc，表示要创建的子进程数
    nproc = atol(argv[1]);  /* Process count */

    // 分配内存空间
    stacks = CHECKALLOC(malloc(nproc * sizeof(void *)), "malloc");
    pids = CHECKALLOC(malloc(nproc * sizeof(pid_t)), "malloc");

    for (i = 0; i < nproc; i++) {
        char *stackTop; /* End of stack buffer */
        stacks[i] = CHECKALLOC(malloc(STACK_SIZE), "stack malloc");
        // 得到栈顶位置
        stackTop = stacks[i] + STACK_SIZE;

        /*
         * 创建子进程
         * 第一个标志表示在子进程清除线程组ID（TID），目的是为了避免子进程与父进程或其他子进程的线程组ID冲突
         * 第二个表示告诉在子进程中设置线程ID，目的是为了允许父进程在子进程中追踪线程
         * 告诉 clone 系统调用在子进程中重新安装信号处理程序，目的是为了允许子进程捕获和处理信号，而不是传递给父进程。
         */
        pids[i] = clone(childFunc, stackTop, CLONE_CHILD_CLEARTID | CLONE_CHILD_SETTID | SIGCHLD, NULL);
        if (pids[i] == -1)
            ERREXIT("clone");
        printf("clone() returned %ld\n", (long)pids[i]);

    }

    sleep(1);

    // 等待所有子进程
    for (i = 0; i < nproc; i++) {
        // 第一个参数为子进程id，第二个参数表示不关心子进程的退出状态，第三个参数表示等待任何子进程
        if (waitpid(pids[i], NULL, 0) == -1)
            ERREXIT("waitpid");
        printf("child %ld has terminated\n", (long)pids[i]);

    }

    // 回收内存空间
    for (i = 0; i < nproc; i++)
        free(stacks[i]);
    free(stacks);
    free(pids);
    return EXIT_SUCCESS;

}
