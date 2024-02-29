// 定义结构体 Frame， 表示递归调用栈的帧
typedef struct {
  int pc, n; // 程序计数器和参数n
  char from, to, via; // 参数from，to，via分别表示起始柱，目标柱和辅助柱
} Frame;

// 宏定义call，用于进行递归调用
#define call(...) ({ *(++top) = (Frame) { .pc = 0, __VA_ARGS__ }; })
// 宏定义ret，用于返回上一层递归调用
#define ret()     ({ top--; })
// 宏定义goto，用于跳转到指定位置
#define goto(loc) ({ f->pc = (loc) - 1; })

void hanoi(int n, char from, char to, char via) {
  // 生命Frame数组stk作为递归调用栈，初始化top为stk数组的头部前一位
  Frame stk[64], *top = stk - 1;

  // 调用宏定义call，将当前递归调用的信息入栈
  call(n, from, to, via);
  for (Frame *f; (f = top) >= stk; f->pc++) {
    switch (f->pc) {
      case 0: if (f->n == 1) { printf("%c -> %c\n", f->from, f->to); goto(4); } break;
      case 1: call(f->n - 1, f->from, f->via, f->to);   break;
      case 2: call(       1, f->from, f->to,  f->via);  break;
      case 3: call(f->n - 1, f->via,  f->to,  f->from); break;
      case 4: ret();                                    break;
      default: assert(0);
    }
  }
}
