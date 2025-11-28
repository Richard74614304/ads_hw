#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>

// ==================== Heap-related structures and functions ====================
// Heap node: stores cost of single purchase, product index, coupon index
typedef struct HeapNode {
    long long cost;  // Actual cost of single purchase (product price - coupon value)
    int p_idx;       // Product index (for price array sorted in ascending order)
    int c_idx;       // Coupon index (for coupon array sorted in descending order)
} HeapNode;

// Min-heap comparison function
int compare_heap(const void *a, const void *b) {
    HeapNode *nodeA = (HeapNode *)a;
    HeapNode *nodeB = (HeapNode *)b;
    if (nodeA->cost > nodeB->cost) return 1;
    if (nodeA->cost < nodeB->cost) return -1;
    return 0;
}

// Heap insert operation
void heap_insert(HeapNode *heap, int *size, HeapNode node) {
    heap[*size] = node;
    int i = *size;
    (*size)++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (compare_heap(&heap[i], &heap[parent]) < 0) {
            HeapNode temp = heap[i];
            heap[i] = heap[parent];
            heap[parent] = temp;
            i = parent;
        } else {
            break;
        }
    }
}

// Heap extract min operation
HeapNode heap_extract_min(HeapNode *heap, int *size) {
    HeapNode min_node = heap[0];
    (*size)--;
    heap[0] = heap[*size];  // Replace root with last element
    int i = 0;
    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;
        if (left < *size && compare_heap(&heap[left], &heap[smallest]) < 0) {
            smallest = left;
        }
        if (right < *size && compare_heap(&heap[right], &heap[smallest]) < 0) {
            smallest = right;
        }
        if (smallest != i) {
            HeapNode temp = heap[i];
            heap[i] = heap[smallest];
            heap[smallest] = temp;
            i = smallest;
        } else {
            break;
        }
    }
    return min_node;
}

// ==================== General comparison functions ====================
// Price ascending comparison (for qsort)
int compare_asc(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

// Coupon descending comparison (for qsort)
int compare_desc(const void *a, const void *b) {
    return *(const int *)b - *(const int *)a;
}

// ==================== Safe memory allocation (avoid null pointer) ====================
void *safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        perror("malloc failed");
        exit(1);
    }
    return ptr;
}

int main(int argc, char** argv) {
    // ==================== File reading configuration ====================
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <testfile>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Failed to open file");
        return 1;
    }
    // Set file buffer to optimize reading speed
    setvbuf(fp, NULL, _IOFBF, 8192);

    int N;          // Number of products/coupons
    long long D;    // Initial budget
    if (fscanf(fp, "%d %lld", &N, &D) != 2 || N < 0 || D < 0) {
        fprintf(stderr, "Error: Invalid N/D (must be non-negative)\n");
        fclose(fp);
        return 1;
    }

    // Special case: no products or no budget
    if (N == 0 || D == 0) {
        printf("0 %lld\n", D);
        printf("Heap greedy algorithm wall time: 0.000000 seconds\n");
        fclose(fp);
        return 0;
    }

    // Dynamic memory allocation (replace VLA to avoid stack overflow)
    int *prices = (int *)safe_malloc(N * sizeof(int));
    int *coupons = (int *)safe_malloc(N * sizeof(int));

    // Read and validate product prices
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d", &prices[i]) != 1 || prices[i] < 0) {
            fprintf(stderr, "Error: Invalid price at index %d (non-negative required)\n", i);
            fclose(fp);
            free(prices);
            free(coupons);
            return 1;
        }
    }
    qsort(prices, N, sizeof(int), compare_asc);  // Sort prices ascending (prioritize cheaper)

    // Read and validate coupons
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d", &coupons[i]) != 1 || coupons[i] < 0) {
            fprintf(stderr, "Error: Invalid coupon at index %d (non-negative required)\n", i);
            fclose(fp);
            free(prices);
            free(coupons);
            return 1;
        }
    }
    qsort(coupons, N, sizeof(int), compare_desc);  // Sort coupons descending (prioritize larger)

    fclose(fp);

    // ==================== Core heap greedy algorithm ====================
    struct timeval wall_start, wall_end;
    gettimeofday(&wall_start, NULL);  // Start timing

    // Heap initialization: max capacity N*N (each product with N coupons)
    HeapNode *heap = (HeapNode *)safe_malloc((N * N + 10000) * sizeof(HeapNode));
    int heap_size = 0;

    // Initial state: each product paired with 0th coupon (largest coupon) inserted into heap
    for (int j = 0; j < N; j++) {
        HeapNode node = {
            .cost = (long long)prices[j] - coupons[0],  // Cost of single purchase
            .p_idx = j,
            .c_idx = 0
        };
        // Avoid negative cost (set to 0 if coupon exceeds price)
        if (node.cost < 0) node.cost = 0;
        heap_insert(heap, &heap_size, node);
    }

    long long total_cost = 0;
    int item_count = 0;

    // Prioritize purchasing the "minimum cost" product-coupon combination
    while (heap_size > 0) {
        HeapNode min_node = heap_extract_min(heap, &heap_size);

        // Stop if current combination exceeds remaining budget
        if (total_cost + min_node.cost > D) {
            break;
        }

        // Purchase this combination
        total_cost += min_node.cost;
        item_count++;

        // Try next coupon (smaller one) for this product, insert into heap
        if (min_node.c_idx + 1 < N) {
            HeapNode next_node = {
                .cost = (long long)prices[min_node.p_idx] - coupons[min_node.c_idx + 1],
                .p_idx = min_node.p_idx,
                .c_idx = min_node.c_idx + 1
            };
            if (next_node.cost < 0) next_node.cost = 0;  // Cost is non-negative
            heap_insert(heap, &heap_size, next_node);
        }
    }

    // Use remaining budget to buy cheapest products at original price (no coupons left)
    if (total_cost < D) {
        long long cheapest = prices[0];
        if (cheapest > 0) {  // Avoid division by zero
            long long buyable = (D - total_cost) / cheapest;
            item_count += buyable;
            total_cost += buyable * cheapest;
        } else {
            // Free products available, buy all remaining products
            item_count += (N - item_count);
            total_cost = D;
            item_count = N;
        }
    }

    gettimeofday(&wall_end, NULL);  // End timing

    // ==================== Result output ====================
    long wall_sec = wall_end.tv_sec - wall_start.tv_sec;
    long wall_usec = wall_end.tv_usec - wall_start.tv_usec;
    double wall_time = wall_sec + (double)wall_usec / 1000000;

    printf("%d %lld\n", item_count, D - total_cost);
    printf("Heap greedy algorithm wall time: %.6f seconds\n", wall_time);

    // Free memory
    free(prices);
    free(coupons);
    free(heap);

    return 0;
}