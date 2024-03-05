#include "thread.h"

#define A 1
#define B 2

#define BARRIER __sync_synchronize() // 用于确保指令重排序时不会破坏程序的预期行为

atomic_int nested;  // 验证同时只有一个进程进入过临界区
atomic_long count;  // 统计有多少个进程进入过临界区

void critical_section() {
  long cnt = atomic_fetch_add(&count, 1);   // 原子操作返回的是执行假发操作前的值
  int i = atomic_fetch_add(&nested, 1) + 1;
  if (i != 1) {
    printf("%d threads in the critical section @ count=%ld\n", i, cnt);
    assert(0);
  }
  atomic_fetch_add(&nested, -1);
}

int volatile x = 0, y = 0, turn;

void TA() {
  while (1) {
    x = 1;                   BARRIER;
    turn = B;                BARRIER; // <- this is critcal for x86
    while (1) {
      if (!y) break;         BARRIER;
      if (turn != B) break;  BARRIER;
    }
    critical_section();
    x = 0;                   BARRIER;
  }
}

void TB() {
  while (1) {
    y = 1;                   BARRIER;
    turn = A;                BARRIER;
    while (1) {
      if (!x) break;         BARRIER;
      if (turn != A) break;  BARRIER;
    }
    critical_section();
    y = 0;                   BARRIER;
  }
}

int main() {
  create(TA);
  create(TB);
  return 0;
}
