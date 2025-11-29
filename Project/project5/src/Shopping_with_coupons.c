#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>
// ==================== Heap-related structures and functions ====================
// Heap node: stores cost of single purchase, product index, coupon index
typedef struct {
    long long cost;
    int p_idx;  // Product index
    int c_idx;  // Coupon index
} HeapNode;

// Min heap implementation
#define HEAP_INIT_CAP 1024
typedef struct {
    HeapNode *data;
    int size;
    int capacity;
} MinHeap;

static inline MinHeap* heap_create() {
    MinHeap *heap = malloc(sizeof(MinHeap));
    heap->capacity = HEAP_INIT_CAP;
    heap->data = malloc(sizeof(HeapNode) * heap->capacity);
    heap->size = 0;
    return heap;
}

static inline void heap_resize(MinHeap *heap) {
    heap->capacity *= 2;
    heap->data = realloc(heap->data, sizeof(HeapNode) * heap->capacity);
}

static inline void heap_swap(HeapNode *a, HeapNode *b) {
    HeapNode t = *a; *a = *b; *b = t;
}

static inline void heap_push(MinHeap *heap, long long cost, int p, int c) {
    if (heap->size >= heap->capacity) heap_resize(heap);
    int i = heap->size++;
    heap->data[i] = (HeapNode){cost, p, c};
    // percolate up
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap->data[i].cost < heap->data[parent].cost) {
            heap_swap(&heap->data[i], &heap->data[parent]);
            i = parent;
        } else break;
    }
}

static inline HeapNode heap_pop(MinHeap *heap) {
    HeapNode top = heap->data[0];
    heap->data[0] = heap->data[--heap->size];
    // percolate down
    int i = 0;
    while (1) {
        int left = 2*i + 1, right = 2*i + 2;
        int smallest = i;
        if (left < heap->size && heap->data[left].cost < heap->data[smallest].cost)
            smallest = left;
        if (right < heap->size && heap->data[right].cost < heap->data[smallest].cost)
            smallest = right;
        if (smallest != i) {
            heap_swap(&heap->data[i], &heap->data[smallest]);
            i = smallest;
        } else break;
    }
    return top;
}

// ==================== General comparison functions ====================
static inline int cmp_p(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

static inline int cmp_c(const void *a, const void *b) {
    return *(int*)b - *(int*)a;
}

int main(int argc, char** argv) {
    // ==================== File reading configuration ====================
    if (argc != 2) { fprintf(stderr, "Usage: %s <testfile>\n", argv[0]); return 1; }
    FILE *fp = fopen(argv[1], "r");
    if (!fp) { perror("fopen"); return 1; }
    setvbuf(fp, NULL, _IOFBF, 8192); // Large buffer for faster reading

    int N; long long D;
    fscanf(fp, "%d %lld", &N, &D);
    int *prices = malloc(N * sizeof(int));
    int *coupons = malloc(N * sizeof(int));
    for (int i=0; i<N; i++) fscanf(fp, "%d", &prices[i]);
    for (int i=0; i<N; i++) fscanf(fp, "%d", &coupons[i]);
    fclose(fp);

    // Preprocessing: products ascending, coupons descending
    qsort(prices, N, sizeof(int), cmp_p);
    qsort(coupons, N, sizeof(int), cmp_c);

    // ==================== Core optimized heap greedy algorithm ====================

    MinHeap *heap = heap_create();
    // Initial heap: each product paired with the 0th coupon (largest discount)
    for (int i=0; i<N; i++) {
        long long cost = (long long)prices[i] - coupons[0];
        if (cost < 0) cost = 0;
        heap_push(heap, cost, i, 0);
    }

    long long total_cost = 0;
    int item_count = 0;
    
    // Timing start
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Core loop: repeatedly extract the minimum cost combination
    while (heap->size > 0) {
        HeapNode curr = heap_pop(heap);

        // Remaining budget insufficient, terminate
        if (total_cost + curr.cost > D) break;

        // Purchase current combination
        item_count++;
        total_cost += curr.cost;

        // Insert the next coupon for this product (if any)
        if (curr.c_idx + 1 < N) {
            long long new_cost = (long long)prices[curr.p_idx] - coupons[curr.c_idx + 1];
            if (new_cost < 0) new_cost = 0;
            heap_push(heap, new_cost, curr.p_idx, curr.c_idx + 1);
        }

        // Zero-cost optimization: if current cost is 0, can directly loop purchase (no budget consumption)
        while (heap->size > 0 && heap->data[0].cost == 0) {
            HeapNode zero = heap_pop(heap);
            item_count++;
            if (zero.c_idx + 1 < N) {
                long long new_cost = (long long)prices[zero.p_idx] - coupons[zero.c_idx + 1];
                if (new_cost < 0) new_cost = 0;
                heap_push(heap, new_cost, zero.p_idx, zero.c_idx + 1);
            }
        }
    }

    // Use remaining budget to buy the cheapest product at full price
    if (total_cost < D && prices[0] > 0) {
        long long rem = D - total_cost;
        item_count += rem / prices[0];
        total_cost += (rem / prices[0]) * prices[0];
    }

    // Timing end
    gettimeofday(&end, NULL);
    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;

    // ==================== Result output ====================
    printf("%d %lld\n", item_count, D - total_cost);
    printf("Optimized time: %.6f seconds\n", time);

    // Free memory
    free(prices); free(coupons);
    free(heap->data); free(heap);
    return 0;
}