#include <stdio.h>
#include <stdlib.h>
#include "ds.h"

//初始化状态表
table inittable(graph G, int startvertex, int Nv) {
    int i;
    table T = (table)malloc(Nv * sizeof(struct tableentry));
    for (i = 0; i < Nv; i++) {
        T[i].known = 0;
        T[i].dist = Infinity;
        T[i].list = G[i].firstedge;
    }
    //将起始点的距离设置为0
    T[startvertex].dist = 0;
    return T;
}

// Dijkstra算法实现
void Dijkstra_inner(graph G, int startvertex, int Nv, table T) {
    int i, v, w;
    ptrtoadjvnode p;

    // 初始化表
    for (i = 0; i < Nv; i++) {
        T[i].known = 0;
        T[i].dist = Infinity;
        T[i].list = G[i].firstedge;
    }
    T[startvertex].dist = 0;

    // 主循环
    for (;;) {
        // 找到未知顶点中距离最小的顶点
        int min_dist = Infinity;
        v = -1;

        for (i = 0; i < Nv; i++) {
            if (!T[i].known && T[i].dist < min_dist) {
                min_dist = T[i].dist;
                v = i;
            }
        }

        // 如果没有找到合适的顶点，退出循环
        if (v == -1 || min_dist == Infinity) {
            break;
        }

        T[v].known = 1;  // 标记为已知

        // 更新所有邻接顶点的距离
        p = T[v].list;
        while (p != NULL) {
            w = p->adjv;

            if (!T[w].known) {
                if (T[v].dist + p->dist < T[w].dist) {
                    T[w].dist = T[v].dist + p->dist;
                }
            }
            p = p->next;
        }
    }
}

void Dijkstra_optimized(graph G, int startvertex, int Nv, table T) {
    int i, v, w;
    ptrtoadjvnode p;

    // 初始化表
    for (i = 0; i < Nv; i++) {
        T[i].known = 0;
        T[i].dist = Infinity;
        T[i].list = G[i].firstedge;
    }
    T[startvertex].dist = 0;

    heaptype H = initheap(startvertex, 0);

    // 主循环
    while (!isHeapEmpty(H)) {
        // 提取最小距离顶点
        int current_vertex = H->node->vertex;
        int current_dist = H->node->dist;

        // 提取最小节点
        H = extract_min(H);

        if (T[current_vertex].known) continue;
        T[current_vertex].known = 1;
        T[current_vertex].dist = current_dist;

        // 更新所有邻接顶点
        ptrtoadjvnode p = T[current_vertex].list;
        while (p != NULL) {
            int w = p->adjv;
            int new_dist = T[current_vertex].dist + p->dist;

            if (!T[w].known && new_dist < T[w].dist) {
                T[w].dist = new_dist;

                if (isInHeap(H, w)) {
                    heapDecreaseKey(H, w, new_dist);
                }
                else {
                    heapinsert(H, w, new_dist);
                }
            }
            p = p->next;
        }
    }

    freeheap(H);
}

//输出函数
// 修改输出函数，只显示前后各20组结果
void print_shortest_paths(table T, int Nv, int startvertex) {
    printf("Shortest paths from vertex %d (showing first 20 and last 20 vertices):\n", startvertex);

    // 显示前20个顶点
    printf("\nFirst 20 vertices:\n");
    int display_count = (Nv < 20) ? Nv : 20;
    for (int i = 0; i < display_count; i++) {
        if (T[i].dist == Infinity) {
            printf("Vertex %d: unreachable\n", i);
        }
        else {
            printf("Vertex %d: shortest path = %d\n", i, T[i].dist);
        }
    }

    // 如果顶点数超过40，显示省略号并显示最后20个
    if (Nv > 40) {
        printf("\n...\n\nLast 20 vertices:\n");
        for (int i = Nv - 20; i < Nv; i++) {
            if (T[i].dist == Infinity) {
                printf("Vertex %d: unreachable\n", i);
            }
            else {
                printf("Vertex %d: shortest path = %d\n", i, T[i].dist);
            }
        }
    }

    printf("\nTotal vertices: %d\n", Nv);
}


//Dijkstra 算法接口
void Dijkstra(graph G, int startvertex) {
    table T = inittable(G, startvertex, G->nv);
    //Tijkstra_inner(G, startvertex, G->nv, T);
    Dijkstra_optimized(G, startvertex, G->nv, T);
    print_shortest_paths(T, G->nv, startvertex);

    free(T);
}

//简单的自用测试代码
/*
int main() {
    int Nv = 6;  // 顶点数
    int Ne = 9;  // 边数
    
    // 创建边数组 (起点, 终点, 权重)
    ptrtoedge edges = (ptrtoedge)malloc(Ne * sizeof(struct edges));
    
    // 添加边 (示例数据)
    edges[0] = (struct edges){1, 4, 2};
    edges[1] = (struct edges){1, 2, 3};
    edges[2] = (struct edges){2, 1, 3};
    edges[3] = (struct edges){2, 5, 4};
    edges[4] = (struct edges){3, 8, 4};
    edges[5] = (struct edges){3, 10, 5};
    edges[6] = (struct edges){4, 2, 5};
    edges[7] = (struct edges){4, 6, 6};
    edges[8] = (struct edges){5, 3, 6};
    
    // 构建图
    graph G = buildgraph(Nv, Ne, edges);
    
    // 执行Dijkstra算法
    Dijkstra(G, 1);

    free(edges);
    freegraph(G, Nv);

    return 0;
}
*/