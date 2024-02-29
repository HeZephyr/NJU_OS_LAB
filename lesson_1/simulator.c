#include <stdio.h>
#include <unistd.h>

#define REGS_FOREACH(_) _(X) _(Y) // 其实就是调用_(X)和_(Y)函数
// 更新逻辑运算结果
#define RUN_LOGIC	X1 = !X && Y; \
			Y1 = !X && !Y;
#define DEFINE(X) 	static int X, X##1; // 定义变量X和X1，其中##起拼接作用
#define UPDATE(X) 	X = X##1;
#define PRINT(X)	printf(#X " = %d; ", X); // 打印，其中#X则是可以理解为打印变量名

int main() {
	REGS_FOREACH(DEFINE);
	while(1) {
		RUN_LOGIC; // 运行逻辑电路，并将结果存储在临时变量中
		REGS_FOREACH(PRINT); // 打印当前寄存器的值
		REGS_FOREACH(UPDATE); // 更新寄存器的值
		putchar('\n');
		sleep(1); // 休眠1秒，模拟数字电路时钟周期
	}
	return 0;
}
