# ads_hw
```
这个仓库用于记录一下我作为一个代码小白学习ads的历程
仓库中包含我做的一些题目以及大作业等等
目前还在施工中
```
### 目录
#### HW1 : Root of AVL Tree
!!!
    作业一 avltree 相关操作参考了陈昊老师的C语言实现示例
#### HW2 ：Self-printable B+ Tree
!!! 
    作业二只实现了order为3的B+树的插入功能，扩展性较差
    之后有机会会加入删除功能，或者加入order可变的拓展
#### HW3 : 
#### project2
使用不同的堆前需要修改ds.h文件的宏定义：
`#define heaptype PairingHeap`
上面是使用配对堆的示例

编译和运行可以在源代码所在文件夹打开终端输入下面的指令：
编译：

使用配对堆：
`gcc Dijkstra.c Graph.c PairingHeap.c ds.h -o main`

使用斐波那契堆：
`gcc Dijkstra.c Graph.c FibonacciHeap.c ds.h -o main`

运行(Windows系统)：
`.\main.exe`