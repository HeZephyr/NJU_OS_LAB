#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#define NTHREAD 64

// 线程状态
enum { T_FREE = 0, T_LIVE, T_DEAD, };

// 表示线程的结构体
struct thread {
  int id, status;	// 线程表示服和线程状态
  pthread_t thread;	// 表示线程
  void (*entry)(int);	// 线程入口点的函数指针
};

// 线程池和指向线程池当前位置的指针
struct thread tpool[NTHREAD], *tptr = tpool;

// 包装实际线程入口函数的函数
void *wrapper(void *arg) {
  struct thread *thread = (struct thread *)arg;
  thread->entry(thread->id);
  return NULL;
}

// 创建新线程的函数
void create(void *fn) {
  assert(tptr - tpool < NTHREAD);	// 确保线程池没有满
  *tptr = (struct thread) {
    .id = tptr - tpool + 1,		// 为线程分配唯一标识符
    .status = T_LIVE,			// 设置线程状态为T_LIVE
    .entry = fn,			// 设置线程入口函数
  };
  pthread_create(&(tptr->thread), NULL, wrapper, tptr); // 创建新线程
  ++tptr;	// 将指针移动到线程池中的下一个位置
}

// 等待所有活动线程的函数
void join() {
  for (int i = 0; i < NTHREAD; i++) {
    struct thread *t = &tpool[i];
    if (t->status == T_LIVE) {
      pthread_join(t->thread, NULL);	// 等待线程结束
      t->status = T_DEAD;		// 将线程状态更新为T_DEAD
    }
  }
}

// 析构函数，在程序退出时清理资源
__attribute__((destructor)) void cleanup() {
  join();	// 在程序退出时等待所有活动线程结束
}
