#include <stdio.h>
#include <stdlib.h>
#include "ds.h"

graph buildgraph(int Nv, int Ne, ptrtoedge edge){
    graph G = (graph) malloc(Nv * sizeof(struct gnode));
    G->ne = Ne;
    G->nv = Nv;
    int i;
    for (i = 0; i < Nv; i++) {
        G[i].firstedge = NULL;
        G[i].lastedge = NULL;
    }
    //将所有邻接表的初状态设置为NULL，方便判断是否为firstedge
    for (i = 0; i < Ne; i++) {
        int s = edge[i].start;
        int e = edge[i].end;
        ptrtoadjvnode newNode1 = (ptrtoadjvnode) malloc(sizeof(struct adjvnode));
        newNode1->adjv = e;
        newNode1->dist = edge[i].k;
        newNode1->next = NULL;
        //创建新节点
        if (!G[s-1].firstedge) {
            G[s-1].firstedge = newNode1;
            G[s-1].lastedge = newNode1;
        }
        //如果邻接表为空，直接赋值
        else {
            G[s-1].lastedge->next = newNode1;
            G[s-1].lastedge = G[s-1].lastedge->next;
        }
        //如果邻接表不为空，更新lastedge并赋值
        ptrtoadjvnode newNode2 = (ptrtoadjvnode)malloc(sizeof(struct adjvnode));
        newNode2->adjv = s;
        newNode2->dist = edge[i].k;
        newNode2->next = NULL;
        if (!G[e-1].firstedge) {
            G[e-1].firstedge = newNode2;
            G[e-1].lastedge = newNode2;
        }
        else {
            G[e-1].lastedge->next = newNode2;
            G[e-1].lastedge = G[e-1].lastedge->next;
        }
        //对于无向图，添加从终点到起点的边即可
    }
    return G;
}

//图内存释放
void freegraph(graph G, int Nv) {
    if (G == NULL) return;
    
    int i;
    for (i = 0; i < Nv; i++) {
        ptrtoadjvnode current = G[i].firstedge;
        ptrtoadjvnode temp;
        
        // 释放当前顶点的所有邻接节点
        while (current != NULL) {
            temp = current;
            current = current->next;
            free(temp);
        }
        
        // 重置指针
        G[i].firstedge = NULL;
        G[i].lastedge = NULL;
    }
    
    // 释放图结构本身
    free(G);
}