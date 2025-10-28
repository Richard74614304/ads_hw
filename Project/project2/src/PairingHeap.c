#include <stdio.h>
#include <stdlib.h>
#include "ds.h"

// 函数实现部分
heaptype initheap(int vertex, int dist) {
    heaptype new_heap = (heaptype)malloc(sizeof(struct phnode));
    if (new_heap == NULL) {
        return NULL;
    }
    
    heapnode new_node = (heapnode)malloc(sizeof(heapelement));
    if (new_node == NULL) {
        free(new_heap);
        return NULL;
    }
    
    new_node->vertex = vertex;
    new_node->dist = dist;
    
    new_heap->node = new_node;
    new_heap->parent = NULL;
    new_heap->first_child = NULL;
    new_heap->left_sibling = NULL;
    new_heap->right_sibling = NULL;
    
    return new_heap;
}

heaptype merge(heaptype H1, heaptype H2) {
    if (H1 == NULL) return H2;
    if (H2 == NULL) return H1;
    
    // 比较两个堆的根节点距离
    if (H1->node->dist <= H2->node->dist) {
        // H1作为根，H2成为H1的第一个子节点
        H2->parent = H1;
        H2->left_sibling = NULL;
        H2->right_sibling = H1->first_child;
        
        if (H1->first_child != NULL) {
            H1->first_child->left_sibling = H2;
        }
        
        H1->first_child = H2;
        return H1;
    } else {
        // H2作为根，H1成为H2的第一个子节点
        H1->parent = H2;
        H1->left_sibling = NULL;
        H1->right_sibling = H2->first_child;
        
        if (H2->first_child != NULL) {
            H2->first_child->left_sibling = H1;
        }
        
        H2->first_child = H1;
        return H2;
    }
}

heaptype heapinsert(heaptype H, int vertex, int dist) {
    heaptype new_node = initheap(vertex, dist);
    if (new_node == NULL) {
        return H;
    }
    
    if (H == NULL) {
        return new_node;
    }
    
    return merge(H, new_node);
}

// 辅助函数：合并子节点链表 - 简化版本
heaptype combine_siblings(heaptype first_sibling) {
    if (first_sibling == NULL) return NULL;
    if (first_sibling->right_sibling == NULL) return first_sibling;
    
    // 简单实现：从左到右依次合并
    heaptype result = first_sibling;
    heaptype current = first_sibling->right_sibling;
    
    while (current != NULL) {
        heaptype next = current->right_sibling;
        result = merge(result, current);
        current = next;
    }
    
    return result;
}

// 修正的extract_min函数
heaptype extract_min(heaptype H) {
    if (H == NULL) {
        return NULL;
    }
    
    // 保存第一个子节点
    heaptype first_child = H->first_child;
    
    // 合并所有子节点
    heaptype new_heap = combine_siblings(first_child);
    
    // 注意：这里我们只释放堆节点结构，不释放node数据
    // 因为Dijkstra算法需要访问node数据
    free(H);
    
    return new_heap;
}

// 辅助函数：在堆中查找顶点 - 简化版本
int find_vertex_in_heap(heaptype H, int vertex) {
    if (H == NULL) return 0;
    
    if (H->node->vertex == vertex) {
        return 1;
    }
    
    // 检查所有子节点
    heaptype child = H->first_child;
    while (child != NULL) {
        if (find_vertex_in_heap(child, vertex)) {
            return 1;
        }
        child = child->right_sibling;
    }
    
    return 0;
}

int isInHeap(heaptype H, int vertex) {
    return find_vertex_in_heap(H, vertex);
}

int isHeapEmpty(heaptype H) {
    return H == NULL;
}

// 辅助函数：查找包含指定顶点的节点
heaptype find_node(heaptype H, int vertex) {
    if (H == NULL) return NULL;
    
    if (H->node->vertex == vertex) {
        return H;
    }
    
    heaptype child = H->first_child;
    while (child != NULL) {
        heaptype found = find_node(child, vertex);
        if (found != NULL) {
            return found;
        }
        child = child->right_sibling;
    }
    
    return NULL;
}

// 辅助函数：将节点从父节点中分离
void detach_from_parent(heaptype node) {
    if (node == NULL || node->parent == NULL) {
        return;
    }
    
    heaptype parent = node->parent;
    
    // 如果节点是父节点的第一个子节点
    if (parent->first_child == node) {
        parent->first_child = node->right_sibling;
        if (node->right_sibling != NULL) {
            node->right_sibling->left_sibling = NULL;
        }
    } else {
        // 节点不是第一个子节点
        if (node->left_sibling != NULL) {
            node->left_sibling->right_sibling = node->right_sibling;
        }
        if (node->right_sibling != NULL) {
            node->right_sibling->left_sibling = node->left_sibling;
        }
    }
    
    node->parent = NULL;
    node->left_sibling = NULL;
    node->right_sibling = NULL;
}

// 修正的heapDecreaseKey函数
void heapDecreaseKey(heaptype H, int vertex, int new_dist) {
    if (H == NULL) return;
    
    // 查找目标节点
    heaptype target = find_node(H, vertex);
    if (target == NULL) return;
    
    if (new_dist >= target->node->dist) {
        return; // 新距离不比原来小，不需要操作
    }
    
    // 更新距离
    target->node->dist = new_dist;
    
    // 如果节点不是根节点且新距离小于父节点距离，需要切割
    if (target->parent != NULL && new_dist < target->parent->node->dist) {
        // 保存当前堆的根
        heaptype root = H;
        while (root->parent != NULL) {
            root = root->parent;
        }
        
        // 从父节点中分离
        detach_from_parent(target);
        
        // 将分离的节点与根合并
        if (root != target) {
            target->parent = NULL;
            target->left_sibling = NULL;
            target->right_sibling = root->first_child;
            
            if (root->first_child != NULL) {
                root->first_child->left_sibling = target;
            }
            
            root->first_child = target;
            target->parent = root;
        }
    }
}

void freeheap(heaptype H) {
    if (H == NULL) {
        return;
    }
    
    // 递归释放所有子节点
    heaptype child = H->first_child;
    while (child != NULL) {
        heaptype next_child = child->right_sibling;
        freeheap(child);
        child = next_child;
    }
    
    // 释放当前节点
    if (H->node != NULL) {
        free(H->node);
    }
    free(H);
}