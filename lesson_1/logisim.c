#include <stdio.h>
#include <unistd.h>

// 定义宏，用于迭代所有输入寄存器
#define REGS_FOREACH(_) _(X) _(Y)
// 定义宏，用于迭代所有输出寄存器
#define OUTS_FOREACH(_) _(A) _(B) _(C) _(D) _(E) _(F) _(G)
// 更新逻辑运算结果
#define RUN_LOGIC	X1 = !X && Y; \
		     	Y1 = !X && !Y; \
	   		A = (!X && !Y) || (X && !Y); \
			B = 1; \
			C = (!X && !Y) || (!X && Y); \
			D = (!X && !Y) || (X && !Y); \
			E = (!X && !Y) || (X && !Y); \
			F = (!X && !Y); \
			G = (X && !Y);
#define DEFINE(X) static int X, X##1;
#define UPDATE(X) X = X##1;
#define PRINT(X)  printf(#X " = %d; ", X);

int main() {
	REGS_FOREACH(DEFINE);
	OUTS_FOREACH(DEFINE);
	while (1) {
		RUN_LOGIC;
		OUTS_FOREACH(PRINT);
		REGS_FOREACH(UPDATE);
		putchar('\n');
		// 刷新标准输出缓冲区，确保打印及时显示
		fflush(stdout);
		sleep(1);
	}
	return 0;
}
