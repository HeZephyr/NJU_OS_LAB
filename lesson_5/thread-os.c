#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#define MAX_CPU 8

// union允许在统一内存空间中存储不同类型的数据，用于节省内存空间，因为它的大小取决于最大的成员的大小
typedef union task {
  struct {
    const char *name;           // 任务名称
    union task *next;           // 下一个任务的指针
    void      (*entry)(void *); // 任务的入口函数孩子会怎
    Context    *context;        // 上下文结构体指针，保存任务的运行状态
  };
  uint8_t stack[8192];          // 任务的堆栈空间
} Task;

// 保存每一个CPU对应的当前任务
Task *currents[MAX_CPU];

// 定义宏， 获取当前CPU对应的档案任务
#define current currents[cpu_current()]

// user-defined tasks

int locked = 0;
void lock()   { while (atomic_xchg(&locked, 1)); }  // 锁
void unlock() { atomic_xchg(&locked, 0); }          // 解锁

void func(void *arg) {
  while (1) {
    lock();
    printf("Thread-%s on CPU #%d\n", arg, cpu_current());   // 输出当前线程信息
    unlock();
    for (int volatile i = 0; i < 100000; i++) ;             // 空循环，模拟任务执行
  }
}

// 初始化任务
Task tasks[] = {
  { .name = "A", .entry = func },
  { .name = "B", .entry = func },
  { .name = "C", .entry = func },
  { .name = "D", .entry = func },
  { .name = "E", .entry = func },
};

// ------------------

// 中断处理函数
Context *on_interrupt(Event ev, Context *ctx) {
  extern Task tasks[];  // 声明外部的任务数组
  if (!current) current = &tasks[0];    // 如果当前任务为空，则设置为任务数组的第一个任务
  else          current->context = ctx; // 否则更新当前任务的上下文指针
  do {
    current = current->next;    // 切换到下一个任务
  } while ((current - tasks) % cpu_count() != cpu_current());   // 循环直到找到当前CPU对应的任务
  return current->context;
}

void mp_entry() {
  iset(true);   // 设置中断允许
  yield();      // 切换到下一个任务
}

int main() {
  cte_init(on_interrupt);   // 设置中断处理函数

  for (int i = 0; i < LENGTH(tasks); i++) {
    Task *task    = &tasks[i];
    Area stack    = (Area) { &task->context + 1, task + 1 };
    task->context = kcontext(stack, task->entry, (void *)task->name);
    task->next    = &tasks[(i + 1) % LENGTH(tasks)];
  }
  mpe_init(mp_entry);
}
