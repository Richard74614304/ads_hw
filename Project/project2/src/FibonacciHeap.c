#include <stdio.h>
#include <stdlib.h>
#include "ds.h"

// 前向声明所有辅助函数
void link(heaptype parent, heaptype child);
heaptype consolidate(heaptype H);
void cut(heaptype H, heaptype node, heaptype parent);
void cascading_cut(heaptype H, heaptype node);
heaptype find_node(heaptype H, int vertex);

// 创建新节点
heaptype initheap(int vertex, int dist) {
    ptrtofh new_node = (ptrtofh)malloc(sizeof(struct fhnode));
    if (!new_node) return NULL;
    
    new_node->node = (heapnode)malloc(sizeof(heapelement));
    if (!new_node->node) {
        free(new_node);
        return NULL;
    }
    
    new_node->node->vertex = vertex;
    new_node->node->dist = dist;
    new_node->parent = NULL;
    new_node->child = NULL;
    new_node->degree = 0;
    new_node->marked = 0;
    
    // 在斐波那契堆中，单个节点形成一个循环链表
    new_node->left = new_node;
    new_node->right = new_node;
    
    return new_node;
}

// 合并两个斐波那契堆
heaptype merge(heaptype H1, heaptype H2) {
    if (H1 == NULL) return H2;
    if (H2 == NULL) return H1;
    
    // 将H2插入到H1的右边
    ptrtofh H1_right = H1->right;
    ptrtofh H2_left = H2->left;
    
    H1->right = H2;
    H2->left = H1;
    H1_right->left = H2_left;
    H2_left->right = H1_right;
    
    // 返回较小的根节点
    return (H1->node->dist < H2->node->dist) ? H1 : H2;
}

// 插入节点到堆中
heaptype heapinsert(heaptype H, int vertex, int dist) {
    ptrtofh new_node = initheap(vertex, dist);
    if (!new_node) return H;
    
    if (H == NULL) {
        return new_node;
    }
    
    return merge(H, new_node);
}

// 链接两个树（辅助函数）
void link(heaptype parent, heaptype child) {
    // 从根链表中移除child
    child->left->right = child->right;
    child->right->left = child->left;
    
    // 使child成为parent的子节点
    child->parent = parent;
    if (parent->child == NULL) {
        parent->child = child;
        child->left = child;
        child->right = child;
    } else {
        ptrtofh first_child = parent->child;
        ptrtofh last_child = first_child->left;
        
        child->right = first_child;
        child->left = last_child;
        first_child->left = child;
        last_child->right = child;
    }
    
    parent->degree++;
    child->marked = 0;
}

// 合并相同度数的树（辅助函数）
heaptype consolidate(heaptype H) {
    if (H == NULL) return NULL;
    
    int max_degree = 100; // 可以根据需要调整大小
    heaptype degree_table[max_degree];
    for (int i = 0; i < max_degree; i++) {
        degree_table[i] = NULL;
    }
    
    // 收集所有根节点
    int count = 0;
    heaptype nodes_to_process[max_degree];
    heaptype current = H;
    heaptype start = H;
    
    do {
        nodes_to_process[count++] = current;
        current = current->right;
    } while (current != start);
    
    // 处理每个根节点
    for (int i = 0; i < count; i++) {
        heaptype x = nodes_to_process[i];
        int d = x->degree;
        
        while (degree_table[d] != NULL) {
            heaptype y = degree_table[d];
            if (x->node->dist > y->node->dist) {
                heaptype temp = x;
                x = y;
                y = temp;
            }
            
            // 使y成为x的子节点
            link(x, y);
            degree_table[d] = NULL;
            d++;
        }
        degree_table[d] = x;
    }
    
    // 重建根链表
    H = NULL;
    for (int i = 0; i < max_degree; i++) {
        if (degree_table[i] != NULL) {
            degree_table[i]->left = degree_table[i];
            degree_table[i]->right = degree_table[i];
            H = merge(H, degree_table[i]);
        }
    }
    
    return H;
}

// 从堆中提取最小元素 - 返回处理后的根节点
heaptype extract_min(heaptype H) {
    if (H == NULL) return NULL;
    
    heaptype min_node = H;
    
    // 将最小节点的所有子节点添加到根链表中
    if (min_node->child != NULL) {
        heaptype child = min_node->child;
        heaptype first_child = child;
        
        do {
            heaptype next_child = child->right;
            child->parent = NULL;
            child->left = child;
            child->right = child;
            H = merge(H, child);
            child = next_child;
        } while (child != first_child);
    }
    
    // 从根链表中移除最小节点
    if (min_node->right == min_node) {
        // 只有一个根节点
        free(min_node->node);
        free(min_node);
        return NULL;
    }
    
    min_node->left->right = min_node->right;
    min_node->right->left = min_node->left;
    
    heaptype new_root = min_node->right;
    free(min_node->node);
    free(min_node);
    
    // 合并相同度数的树
    new_root = consolidate(new_root);
    
    // 找到新的最小节点
    heaptype current = new_root;
    heaptype min_root = new_root;
    heaptype start = new_root;
    
    do {
        if (current->node->dist < min_root->node->dist) {
            min_root = current;
        }
        current = current->right;
    } while (current != start);
    
    return min_root;
}

// 切断节点与其父节点的连接（辅助函数）
void cut(heaptype H, heaptype node, heaptype parent) {
    // 从父节点的子链表中移除node
    if (node->right == node) {
        parent->child = NULL;
    } else {
        node->left->right = node->right;
        node->right->left = node->left;
        if (parent->child == node) {
            parent->child = node->right;
        }
    }
    
    parent->degree--;
    
    // 将node添加到根链表
    node->parent = NULL;
    node->marked = 0;
    node->left = node;
    node->right = node;
    merge(H, node);
}

// 级联切断（辅助函数）
void cascading_cut(heaptype H, heaptype node) {
    heaptype parent = node->parent;
    if (parent != NULL) {
        if (node->marked == 0) {
            node->marked = 1;
        } else {
            cut(H, node, parent);
            cascading_cut(H, parent);
        }
    }
}

// 在堆中查找节点（辅助函数）
heaptype find_node(heaptype H, int vertex) {
    if (H == NULL) return NULL;
    
    heaptype stack[1000]; // 使用栈进行DFS
    int top = 0;
    stack[top++] = H;
    
    while (top > 0) {
        heaptype current = stack[--top];
        heaptype start = current;
        
        do {
            if (current->node->vertex == vertex) {
                return current;
            }
            
            if (current->child != NULL) {
                stack[top++] = current->child;
            }
            
            current = current->right;
        } while (current != start);
    }
    
    return NULL;
}

// 减小键值
void heapDecreaseKey(heaptype H, int vertex, int new_dist) {
    // 首先需要找到包含该顶点的节点
    heaptype node = find_node(H, vertex);
    if (node == NULL || node->node->dist <= new_dist) {
        return;
    }
    
    node->node->dist = new_dist;
    heaptype parent = node->parent;
    
    if (parent != NULL && node->node->dist < parent->node->dist) {
        cut(H, node, parent);
        cascading_cut(H, parent);
    }
}

// 检查顶点是否在堆中
int isInHeap(heaptype H, int vertex) {
    return find_node(H, vertex) != NULL;
}

// 检查堆是否为空
int isHeapEmpty(heaptype H) {
    return H == NULL;
}

// 获取最小元素（不删除）
Element_heap get_min(heaptype H) {
    if (H == NULL) {
        static heapelement empty = {-1, -1};
        return &empty;
    }
    return H->node;
}

// 释放堆
void freeheap(heaptype H) {
    if (H == NULL) return;
    
    heaptype stack[1000];
    int top = 0;
    stack[top++] = H;
    
    while (top > 0) {
        heaptype current = stack[--top];
        heaptype start = current;
        
        do {
            heaptype next = current->right;
            
            if (current->child != NULL) {
                stack[top++] = current->child;
            }
            
            free(current->node);
            free(current);
            current = next;
        } while (current != start);
    }
}