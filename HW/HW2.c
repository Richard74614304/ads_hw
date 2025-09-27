#include <stdio.h>
#include <stdlib.h>

#define BLANK 114514

typedef struct Bplus3node* ptrtoB;
struct Bplus3node {
    int keycount;
    int isleaf;
    int key[3];
    ptrtoB pointer[3];
    ptrtoB parent;
};

ptrtoB NIL = NULL;

//链表定义和操作
typedef struct queue* ptrtoqueue;
struct queuenode {
    ptrtoB node;
    struct queuenode* next;
};

struct queue {
    struct queuenode* front;
    struct queuenode* rear;
};

ptrtoqueue createqueue() {
    ptrtoqueue Q = (ptrtoqueue) malloc(sizeof(struct queue));
    if (Q == NULL) { perror("malloc"); exit(1); }
    Q -> front = Q -> rear = NULL;
    return Q;
}

void enqueue(ptrtoqueue Q, ptrtoB Tnode) {
    if (Tnode == NIL) return;
    struct queuenode* newnode = (struct queuenode*) malloc(sizeof(struct queuenode));
    if (newnode == NULL) { perror("malloc"); exit(1); }
    newnode -> node = Tnode;
    newnode -> next = NULL;
    if (Q -> front == NULL) {
        Q -> front = Q -> rear = newnode;
        return;
    }
    Q -> rear -> next = newnode;
    Q -> rear = newnode;
}

ptrtoB dequeue(ptrtoqueue Q) {
    if (Q -> front == NULL) {
        return NULL;
    }
    struct queuenode* temp = Q -> front;
    ptrtoB Tnode = temp -> node;
    Q -> front = Q -> front -> next;
    if (Q -> front == NULL)
        Q -> rear = NULL;
    free(temp);
    return Tnode;
}

//寻找子树最小值
int findleast(ptrtoB T) {
    if (T == NIL) return BLANK;
    if (T -> isleaf) {
        return T -> key[0];
    } else {
        return findleast(T -> pointer[0]);
    }
}

//排序
void keysort(int* x, int k) {
    for (int i = 1; i < k; i++) {
        int temp = x[i];
        int j;
        for (j = i; j > 0 && x[j - 1] > temp; j--) {
            x[j] = x[j - 1];
        }
        x[j] = temp;
    }
}

//创建新节点
ptrtoB new_node(int isleaf) {
    ptrtoB p = (ptrtoB) malloc(sizeof(struct Bplus3node));
    if (p == NULL) { perror("malloc"); exit(1); }
    p -> keycount = 0;
    p -> isleaf = isleaf ? 1 : 0;
    p -> parent = NIL;
    for (int i = 0; i < 3; i++) {
        p -> key[i] = 0;
        p -> pointer[i] = NIL;
    }
    return p;
}

//将插入的关键字放入正确位置
void keyplace(ptrtoB T, int x) {
    if (T -> keycount != 1 && T -> keycount != 2) {
        printf ("error keyplace\n");
        return;
    }
    T -> key[T -> keycount] = x;
    T -> keycount++;
    keysort(T -> key, T -> keycount);
}

//自打印
void selfprint(ptrtoB T) {
    if (T == NIL) return;
    ptrtoqueue Q1 = createqueue();
    ptrtoqueue Q2 = createqueue();
    enqueue(Q1, T);
    while (Q1 -> front != NULL || Q2 -> front != NULL) {
        if (Q1 -> front != NULL) {
            while (Q1 -> front != NULL) {
                ptrtoB Tnode = dequeue(Q1);
                if (Tnode == NULL) continue;
                printf("[%d", Tnode -> key[0]);
                int k = Tnode -> keycount;
                for (int i = 1; i < k; i++) {
                    printf(",%d", Tnode -> key[i]);
                }
                printf("]");
                if (!Tnode -> isleaf) {
                    for (int i = 0; i <= k; i++) {
                        if (Tnode -> pointer[i] != NIL) {
                            enqueue(Q2, Tnode -> pointer[i]);
                        }
                    }
                }
            }
            printf("\n");
        } else if (Q2 -> front != NULL) {
            while (Q2 -> front != NULL) {
                ptrtoB Tnode = dequeue(Q2);
                if (Tnode == NULL) continue;
                printf("[%d", Tnode -> key[0]);
                int k = Tnode -> keycount;
                for (int i = 1; i < k; i++) {
                    printf(",%d", Tnode -> key[i]);
                }
                printf("]");
                if (!Tnode -> isleaf) {
                    for (int i = 0; i <= k; i++) {
                        if (Tnode -> pointer[i] != NIL) {
                            enqueue(Q1, Tnode -> pointer[i]);
                        }
                    }
                }
            }
            printf("\n");
        }
    }
    free(Q1);
    free(Q2);
}

//释放B+树空间
void Bplusfree(ptrtoB T) {
    if (T == NULL) return;
    if (!T -> isleaf) {
        for (int i = 0; i <= T -> keycount; i++) {
            Bplusfree(T -> pointer[i]);
        }
    }
    free(T);
}

//B+树插入
ptrtoB insert(ptrtoB root, int x) {
    if (root == NULL) {
        ptrtoB R = new_node(1);
        R -> key[0] = x;
        R -> keycount = 1;
        R -> isleaf = 1;
        R -> parent = NIL;
        for (int i = 0; i < 3; i++) R -> pointer[i] = NIL;
        return R;
    }

    ptrtoB cur = root;
    //寻找合适的叶节点
    while (!cur -> isleaf) {
        int i;
        for (i = 0; i < cur -> keycount; i++) {
            if (x < cur -> key[i]) break;
        }
        cur = cur -> pointer[i];
    }

    //排除插入节点已存在的情况
    for (int i = 0; i < cur -> keycount; i++) {
        if (cur -> key[i] == x) {
            printf("Key %d is duplicated\n", x);
            return root;
        }
    }

    //留有空位，可以直接插入
    if (cur -> keycount < 3) {
        cur -> key[cur -> keycount] = x;
        cur -> keycount++;
        keysort(cur -> key, cur -> keycount);
        return root;
    }

    //split
    //先做key的重排
    int tempkey[4];
    for (int i = 0; i < 3; i++) tempkey[i] = cur -> key[i];
    tempkey[3] = x;
    keysort(tempkey, 4);

    //向右分裂
    ptrtoB left = cur;
    ptrtoB right = new_node(1);
    left -> key[0] = tempkey[0];
    left -> key[1] = tempkey[1];
    left -> keycount = 2;
    right -> key[0] = tempkey[2];
    right -> key[1] = tempkey[3];
    right -> keycount = 2;
    for (int i = 0; i < 3; i++) {
        left -> pointer[i] = NIL;
        right -> pointer[i] = NIL;
    }
    ptrtoB parent = left -> parent;
    left -> parent = parent;
    right -> parent = parent;
    ptrtoB child_left = left;
    ptrtoB child_right = right;

    //重复调整结构
    while (1) {
        //con.1 分裂节点无父节点
        if (parent == NIL) {
            ptrtoB newroot = new_node(0);
            newroot -> isleaf = 0;
            newroot -> pointer[0] = child_left;
            newroot -> pointer[1] = child_right;
            newroot -> pointer[2] = NIL;
            newroot -> key[0] = findleast(child_right);
            newroot -> keycount = 1;
            child_left -> parent = newroot;
            child_right -> parent = newroot;
            return newroot;
        }

        //寻找分裂节点在父节点指针数组的index
        int idx = -1;
        for (int i = 0; i <= parent -> keycount; i++) {
            if (parent -> pointer[i] == child_left) {
                idx = i;
                break;
            }
        }

        //为了防止树结构被破坏，保护代码正常运行
        if (idx == -1) {
            idx = parent -> keycount;
        }

        //将分裂出的新节点right插入分裂节点右边
        ptrtoB temp_ptrs[4];
        for (int i = 0; i <= parent -> keycount; i++) temp_ptrs[i] = parent -> pointer[i];
        temp_ptrs[parent -> keycount + 1] = NIL;
        for (int i = parent -> keycount + 1; i > idx + 1; i--) {
            temp_ptrs[i] = temp_ptrs[i - 1];
        }
        temp_ptrs[idx + 1] = child_right;

        //判断父节点是否需要继续分裂
        int new_ptrs_count = parent -> keycount + 2;
        if (new_ptrs_count <= 3) {
            //无需继续分裂，将指针写回父节点
            for (int i = 0; i < 3; i++) parent -> pointer[i] = NIL;
            for (int i = 0; i < new_ptrs_count; i++) parent -> pointer[i] = temp_ptrs[i];
            parent -> keycount = new_ptrs_count - 1;
            for (int i = 0; i < parent -> keycount; i++) {
                parent -> key[i] = findleast(parent -> pointer[i + 1]);
            }
            for (int i = 0; i < new_ptrs_count; i++) {
                if (parent -> pointer[i] != NIL) parent -> pointer[i] -> parent = parent;
            }
            return root;
        }

        //需要继续分裂
        ptrtoB left_parent = parent;
        ptrtoB right_parent = new_node(0);
        left_parent -> isleaf = 0;
        right_parent -> isleaf = 0;
        for (int i = 0; i < 3; i++) {
            left_parent -> pointer[i] = NIL;
            right_parent -> pointer[i] = NIL;
        }
        left_parent -> pointer[0] = temp_ptrs[0];
        left_parent -> pointer[1] = temp_ptrs[1];
        left_parent -> keycount = 1;
        left_parent -> key[0] = findleast(left_parent -> pointer[1]);
        right_parent -> pointer[0] = temp_ptrs[2];
        right_parent -> pointer[1] = temp_ptrs[3];
        right_parent -> keycount = 1;
        right_parent -> key[0] = findleast(right_parent -> pointer[1]);

        //更新parent值
        if (left_parent -> pointer[0] != NIL) left_parent -> pointer[0] -> parent = left_parent;
        if (left_parent -> pointer[1] != NIL) left_parent -> pointer[1] -> parent = left_parent;
        if (right_parent -> pointer[0] != NIL) right_parent -> pointer[0] -> parent = right_parent;
        if (right_parent -> pointer[1] != NIL) right_parent -> pointer[1] -> parent = right_parent;
        
        //向上迭代，进入新循环
        child_left = left_parent;
        child_right = right_parent;
        parent = child_left -> parent;
    }
}

int main() {
    int k;
    if (scanf("%d", &k) != 1) return 0;
    ptrtoB T = NULL;
    for (int i = 0; i < k; i++) {
        int x;
        if (scanf("%d", &x) != 1) break;
        T = insert(T, x);
    }

    selfprint(T);
    Bplusfree(T);
    return 0;
}
