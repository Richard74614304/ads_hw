#include <stdio.h>
#include <stdlib.h>

typedef struct avlnode* ptrtoavl;
struct avlnode {
    int key;
    ptrtoavl left;
    ptrtoavl right;
    int height;
};

int Height(ptrtoavl T) {
    if (T == NULL) {
        return -1;
    }
    else 
        return T -> height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

ptrtoavl slr(ptrtoavl T) {
    ptrtoavl CT = T -> right;
    T -> right = CT -> left;
    CT -> left = T;
    T -> height = max(Height(T -> left), Height(T -> right)) + 1;
    CT -> height = max(Height(CT -> left), Height(CT -> right)) + 1;
    return CT;
}

ptrtoavl srr(ptrtoavl T) {
    ptrtoavl CT = T -> left;
    T -> left = CT -> right;
    CT -> right = T;
    T -> height = max(Height(T -> left), Height(T -> right)) + 1;
    CT -> height = max(Height(CT -> left), Height(CT -> right)) + 1;
    return CT;
}

ptrtoavl drlr(ptrtoavl T) {
    T -> right = srr(T -> right);
    T = slr(T);
    return T;
}

ptrtoavl dlrr(ptrtoavl T) {
    T -> left = slr(T -> left);
    T = srr(T);
    return T;
}

ptrtoavl insert(ptrtoavl T, int x) {
    if (T == NULL) {
        T = (ptrtoavl) malloc(sizeof(struct avlnode));
        T -> key = x;
        T -> left = NULL;
        T -> right = NULL;
        T -> height = 0;
    }
    else if (T -> key < x) {
        T -> right = insert(T -> right, x);
        if (Height(T -> left) - Height(T -> right) == -2) {
            if (T -> right -> key < x) {
                T = slr(T);
            }
            else if (T -> right -> key > x) {
                T = drlr(T);
            }
        }
    }
    else if (T -> key > x) {
        T -> left = insert(T ->left, x);
        if (Height(T -> left) - Height(T -> right) == 2) {
            if (T -> left -> key > x) {
                T = srr(T);
            }
            else if (T -> left -> key < x) {
                T = dlrr(T);
            }
        }
    }

    T -> height = max(Height(T -> left), Height(T -> right)) + 1;
    return T;
}

void freeavl(ptrtoavl T) {
    if (T == NULL) {
        return;
    }
    freeavl(T -> left);
    freeavl(T -> right);
    free(T);
}

int main() {
    int k;
    ptrtoavl avltree = NULL;
    scanf("%d", &k);
    for (int i = 0; i < k; i++) {
        int Key;
        scanf("%d", &Key);
        avltree = insert(avltree, Key);
    }

    printf("%d", avltree -> key);
    freeavl(avltree);
    return 0;
}