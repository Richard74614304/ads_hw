#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "TP.h"

// Sorting standards (0 for weight, 1 for height, 2 for area)
#define SORT_STANDARD 2

int decmp_w(const void* a, const void* b) {
    const Itemptr x = (const Itemptr)a;
    const Itemptr y = (const Itemptr)b;
    if (x->w < y->w) return 1;
    else if (x->w > y->w) return -1;
    else return 0;
}
int decmp_h(const void* a, const void* b) {
    const Itemptr x = (const Itemptr)a;
    const Itemptr y = (const Itemptr)b;
    if (x->h < y->h) return 1;
    else if (x->h > y->h) return -1;
    else return 0;
}
int decmp_a(const void* a, const void* b) {
    const Itemptr x = (const Itemptr)a;
    const Itemptr y = (const Itemptr)b;
    if (x->w * x->h < y->w * y->h) return 1;
    else if (x->w * x->h > y->w * y->h) return -1;
    else return 0;
}

void FFD_R_sort(Items items, int n) {
    if (SORT_STANDARD == 0) {
        qsort(items, n, sizeof(struct Item), decmp_w);
    }
    else if (SORT_STANDARD == 1) {
        qsort(items, n, sizeof(struct Item), decmp_h);
    }
    else if (SORT_STANDARD == 2) {
        qsort(items, n, sizeof(struct Item), decmp_a);
    }
    return;
}

// merge adjacent free regions
void merge_free_regions(FreeRegion* free_regions, int* free_count) {
    int merged = 1;
    while (merged) {
        merged = 0;
        for (int i = 0; i < *free_count; i++) {
            for (int j = i + 1; j < *free_count; j++) {
                FreeRegion* a = &free_regions[i];
                FreeRegion* b = &free_regions[j];

                // condition 1: horizontal merge (side by side)
                if (a->y == b->y && a->height == b->height && (a->x + a->width) == b->x) {
                    a->width += b->width;
                    free_regions[j] = free_regions[*free_count - 1];
                    (*free_count)--;
                    merged = 1;
                    break; // restart after merging
                }

                // condition 2: vertical merge (top-down, optional)
                if (a->x == b->x && a->width == b->width && (a->y + a->height) == b->y) {
                    a->height += b->height;
                    free_regions[j] = free_regions[*free_count - 1];
                    (*free_count)--;
                    merged = 1;
                    break;
                }
            }
            if (merged) break;
        }
    }
}

int FFD_R(int W, int n, Items items) {
    FFD_R_sort(items, n);
    int bin_height = 0; // current height of the container
    FreeRegion* free_regions = NULL; // array of free regions
    int free_count = 0; // number of free regions
    int free_capacity = 10; // initial capacity of the free regions array

    // allocate initial memory for free regions
    free_regions = (FreeRegion*)malloc(free_capacity * sizeof(FreeRegion));
    if (free_regions == NULL) {
        perror("Failed to allocate free regions");
        exit(EXIT_FAILURE);
    }

    // initialize with one big free region
    free_regions[0] = (FreeRegion){0, 0, W, INT_MAX};
    free_count = 1;

    // traverse each item and try to place it
    for (int i = 0; i < n; i++) {
        Itemptr item = &items[i]; // current item to place
        int placed = 0; // if the item has been placed
        int best_region_idx = -1; // index of the best free region
        int best_w = 0, best_h = 0; // index of best placement dimensions
        int best_rotated = 0; // best rotated state

        // candidate 1: unrotated
        int candidate_w1 = item->w;
        int candidate_h1 = item->h;
        // candidate 2: rotated 90 degrees (only if rotated width ≤ W)
        int candidate_w2 = item->h;
        int candidate_h2 = item->w;
        int can_rotate = (candidate_w2 <= W); // check if rotated dimensions fit within container width

        // traverse all available regions to find the first-fit region
        for (int r = 0; r < free_count; r++) {
            FreeRegion* region = &free_regions[r];

            // try candidate 1: unrotated
            if (candidate_w1 <= region->width && candidate_h1 <= region->height) {
                best_region_idx = r;
                best_w = candidate_w1;
                best_h = candidate_h1;
                best_rotated = 0;
                placed = 1;
                break; // first-fit, immediately break
            }

            // try candidate 2: rotated
            if (can_rotate && candidate_w2 <= region->width && candidate_h2 <= region->height) {
                best_region_idx = r;
                best_w = candidate_w2;
                best_h = candidate_h2;
                best_rotated = 1;
                placed = 1;
                break; // first-fit, immediately break
            }
        }

        // place te item and update free regions
        if (placed) {
            // get the best region
            FreeRegion* best_region = &free_regions[best_region_idx];

            // uptate item placement info
            item->x = best_region->x;
            item->y = best_region->y;
            item->rotated = best_rotated;

            // update bin height
            int item_bottom = item->y + best_h;
            if (item_bottom > bin_height) {
                bin_height = item_bottom;
            }

            // cut the free region into new regions
            FreeRegion new_regions[2];
            int new_count = 0;

            // add free region on the right (if any)
            if (best_w < best_region->width) {
                new_regions[new_count++] = (FreeRegion){
                    best_region->x + best_w,
                    best_region->y,
                    best_region->width - best_w,
                    best_h
                };
            }

            // add free region on the top (if any)
            if (best_h < best_region->height) {
                new_regions[new_count++] = (FreeRegion){
                    best_region->x,
                    best_region->y + best_h,
                    best_region->width,
                    best_region->height - best_h
                };
            }

            // remove the used free region and add new regions
            free_regions[best_region_idx] = free_regions[free_count - 1]; // cover cuurent region with the last one
            free_count--;

            // allocate space for new regions if needed
            for (int nr = 0; nr < new_count; nr++) {
                if (free_count >= free_capacity) {
                    free_capacity *= 2;
                    free_regions = (FreeRegion*)realloc(free_regions, free_capacity * sizeof(FreeRegion));
                    if (free_regions == NULL) {
                        perror("Failed to reallocate free regions");
                        exit(EXIT_FAILURE);
                    }
                }
                free_regions[free_count++] = new_regions[nr];
            }

            // merge adjacent free regions
            merge_free_regions(free_regions, &free_count);
        }
        else {
            // if no fitting region found, place the item at the top of the container
            int final_w, final_h, final_rotated;
            // decide rotation based on width
            if (item->w <= W) {
                final_w = item->w;
                final_h = item->h;
                final_rotated = 0;
            }
            else {
                final_w = item->h;
                final_h = item->w;
                final_rotated = 1;
            }

            // place the item on top
            item->x = 0;
            item->y = bin_height;
            item->rotated = final_rotated;

            // update the bin height
            bin_height += final_h;

            // add free region on the right (if any)
            if (final_w < W) {
                if (free_count >= free_capacity) {
                    free_capacity *= 2;
                    free_regions = (FreeRegion*)realloc(free_regions, free_capacity * sizeof(FreeRegion));
                    if (free_regions == NULL) {
                        perror("Failed to reallocate free regions");
                        exit(EXIT_FAILURE);
                    }
                }
                free_regions[free_count++] = (FreeRegion){
                    final_w,
                    item->y,
                    W - final_w,
                    final_h
                };
            }

            merge_free_regions(free_regions, &free_count);
        }
    }

    // free allocated memory
    free(free_regions);

    // return minimum height
    return bin_height;
}