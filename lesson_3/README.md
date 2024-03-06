# 并发控制：互斥和同步

Scalability：性能的新维度

同一份计算任务，时间和空间会随处理器数量的增长而变化
在sum-scalability.c中很好的说明了这一点

pc.c 用互斥锁实现了同步
pc-check.py用来压力测试
`gcc pc.c -lpthread`
`./a.out 5 | python3 pc-check.py 5`
