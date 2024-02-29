## 代码解释
1. simulator.c，模拟数字逻辑电路
2. 将logisim.c编译运行的结果通过管道传给seven-seg.py
`gcc logisim.c && ./a.out | python3 seven-seg.py`
3. hanoi-nr.c就是将hanoi-r.c递归程序转换为非递归程序

## 笔记
C程序的状态机模型（语义， semantics）
* 状态 = stack frame的列表（每个frame有PC） + 全局变量
* 初始状态 = main(argc, argv)， 全局变量初始化
* 迁移 = 执行top stack frame PC的语句； PC++
	* 函数调用 = push frame(frame.PC = 入口)
	* 函数返回 = pop frame
