# 协程库(libco)
链接libco.so的程序被赋予使用协程库的能力，它和pthread相当类似:
1. co_start(name, func, arg)创建一个新的协程，并返回一个指向struct co的指针(类似于pthread_create)
    * 新创建的协程从函数func开始执行，并传入参数arg。新创建的协程不会立即执行，而是调用co_start的协程继续执行
    * 使用协程的应用程序不需要知道struct co的具体定义，因此请把该定义留在co.c中;框架代码中并没有限定struct co结构体的设计，所以可以自由发挥
2. co_wait(co)表示当前协程需要等待，直到co协程的执行完成后才能继续执行(类似于pthread_join)
    * 在被等待的协程结束后，co_wait()返回前，co_start分配的struct co需要被释放。如果你使用malloc()，需要使用free()释放即可
    * 因此，每个协程只能被co_wait()一次(使用协程库的程序应当保证除了初始协程外，其他协程都必须被co_wait()恰好一次，否则会造成内存泄漏)
3. 协程运行后一直在CPU上执行，直到func函数返回或调用co_yield()使当前运行的协程暂时放弃执行。调用co_yield()会切换到其他协程执行。当系统中有多个可运行的协程时(包括当前协程)，你应当随机选择下一个系统中可运行的协程
4. main()函数的执行也是一个协程，因此可以在main()中调用co_yield()。main()函数返回后，无论有多少协程，进程都将直接终止。

下面是一个协程使用的例子，创建了两个(永不结束的)协程，分别打印a和b。由于co_yield()之后切换到的协程是随机的(可能切换到它自己)，因此可能会看到随机的ab交替出现的序列，例如ababba…

```c
#include <stdio.h>
#include "co.h"

void entry(void *arg) {
    while (1) {
        printf("%s", (const char *)arg);
        co_yield();
    }
}

int main() {
    struct co *co1 = co_start("co1", entry, "a");
    struct co *co2 = co_start("co2", entry, "b");
    co_wait(co1); // never returns
    co_wait(co2);
}
```
