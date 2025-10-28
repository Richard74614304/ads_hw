## project2
使用不同的堆前需要修改ds.h文件的宏定义：
`#define heaptype PairingHeap`
上面是使用配对堆的示例

#### 编译和运行
编译和运行可以在源代码所在文件夹打开终端输入下面的指令：

!!!
    编译：
    (需要先进入src文件夹)
    使用配对堆：
    `g++ test.cpp Dijkstra.c Graph.c PairingHeap.c -I ../include -o ../main`
    
    使用斐波那契堆：
    `g++ test.cpp Dijkstra.c Graph.c FibonacciHeap.c -I ../include -o ../main`
    
    可执行文件会生成在项目目录下
    运行(Windows系统)：
    (需要进入project2文件夹)
    `./main.exe test/your_test_file.txt`
    
    比如：
    `./main.exe test/dense_graph_50.txt`

    具体的测试文件已经添加在test目录下
