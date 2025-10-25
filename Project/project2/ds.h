#include <stdio.h>
#include <stdlib.h>

//both of below are pointers
#define heaptype PairingHeap
#define Element_heap heapnode
/*data structure for graph*/

//顶点的邻接表，其中包含表示权重的dist
typedef struct adjvnode* ptrtoadjvnode;
struct adjvnode{
    int adjv;
    int dist;
    ptrtoadjvnode next;
};

//图的节点，为了方便设置了firstedge和lastedge两个索引
typedef struct gnode* ptrtognode;
struct gnode{
    int nv;
    int ne;
    ptrtoadjvnode firstedge;
    ptrtoadjvnode lastedge;
};
typedef ptrtognode graph;

//设置了一个表示边的结构体，包含起点、终点、权重，适用于有向图和无向图
struct edges{
    int start;
    int k;
    int end;
};
typedef struct edges* ptrtoedge;

/*data structure for Dijkstra Algorithm*/
typedef struct{
    int vertex;
    int dist;
}heapelement;
typedef heapelement* heapnode;

//定义了图中各顶点的状态表，用于Dijkstra算法
struct tableentry{
    ptrtoadjvnode list;
    int known;
    int dist;
};
typedef struct tableentry* table;
typedef struct tableentry* ptrtot;

/*data structure for Pairing Heap*/
typedef struct phnode* ptrtoph;
typedef struct phnode* PairingHeap;
struct phnode{
    Element_heap node;
    ptrtoph parent;
    ptrtoph first_child;
    ptrtoph left_sibling;
    ptrtoph right_sibling;
};

/*data structure for Fibonacci Heap*/
typedef struct fhnode* ptrtofh;
typedef struct fhnode* FibonacciHeap;
struct fhnode{
    Element_heap node;
    ptrtofh parent;
    ptrtofh child;
    ptrtofh left;
    ptrtofh right;
    int degree;
    int marked;
};

/*function declaration*/
//graph operation
graph buildgraph(int Nv, int Ne, ptrtoedge edge);
void freegraph(graph G, int Nv);

//heap operation
heaptype initheap(int vertex, int dist);
heaptype merge(heaptype H1, heaptype H2);
heaptype heapinsert(heaptype H, int vertex, int dist);
heaptype extract_min(heaptype H);
void heapDecreaseKey(heaptype H, int vertex, int new_dist);
int isInHeap(heaptype H, int vertex);
int isHeapEmpty(heaptype H);
void freeheap(heaptype H);