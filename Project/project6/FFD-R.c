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

// 新增：合并可用区域的函数
void merge_free_regions(FreeRegion* free_regions, int* free_count) {
    int merged = 1;
    while (merged) { // 循环合并，直到没有可合并的
        merged = 0;
        for (int i = 0; i < *free_count; i++) {
            for (int j = i + 1; j < *free_count; j++) {
                FreeRegion* a = &free_regions[i];
                FreeRegion* b = &free_regions[j];

                // 条件1：水平合并（左右拼）
                if (a->y == b->y && a->height == b->height && (a->x + a->width) == b->x) {
                    // 合并成新区域：x=a.x, y=a.y, 宽=a.w+b.w, 高=a.h
                    a->width += b->width;
                    // 移除j位置的区域（用最后一个覆盖，数量减1）
                    free_regions[j] = free_regions[*free_count - 1];
                    (*free_count)--;
                    merged = 1;
                    break; // 合并后重新遍历
                }

                // 条件2：垂直合并（上下拼，可选）
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
    int bin_height = 0; // 容器当前高度
    FreeRegion* free_regions = NULL; // 可用区域数组
    int free_count = 0; // 可用区域数量
    int free_capacity = 10; // 可用区域数组初始容量

    // 分配初始可用区域数组内存
    free_regions = (FreeRegion*)malloc(free_capacity * sizeof(FreeRegion));
    if (free_regions == NULL) {
        perror("Failed to allocate free regions");
        exit(EXIT_FAILURE);
    }

    // 初始可用区域：整个容器（宽度W，高度初始化为极大值，后续动态调整）
    free_regions[0] = (FreeRegion){0, 0, W, INT_MAX};
    free_count = 1;

    // Step 3: 遍历每个物品，尝试放置（首次适配+旋转）
    for (int i = 0; i < n; i++) {
        Itemptr item = &items[i]; // 当前待放置物品
        int placed = 0; // 标记是否已放置
        int best_region_idx = -1; // 最佳可用区域索引
        int best_w = 0, best_h = 0; // 最佳放置尺寸（原始/旋转）
        int best_rotated = 0; // 最佳旋转状态

        // 候选状态1：原始尺寸（未旋转）
        int candidate_w1 = item->w;
        int candidate_h1 = item->h;
        // 候选状态2：旋转90度（仅当旋转后宽度≤W时允许）
        int candidate_w2 = item->h;
        int candidate_h2 = item->w;
        int can_rotate = (candidate_w2 <= W); // 检查旋转后是否符合宽度约束

        // 遍历所有可用区域，寻找首次适配的区域
        for (int r = 0; r < free_count; r++) {
            FreeRegion* region = &free_regions[r];

            // 尝试状态1：未旋转
            if (candidate_w1 <= region->width && candidate_h1 <= region->height) {
                best_region_idx = r;
                best_w = candidate_w1;
                best_h = candidate_h1;
                best_rotated = 0;
                placed = 1;
                break; // 首次适配，立即跳出
            }

            // 尝试状态2：旋转（如果允许）
            if (can_rotate && candidate_w2 <= region->width && candidate_h2 <= region->height) {
                best_region_idx = r;
                best_w = candidate_w2;
                best_h = candidate_h2;
                best_rotated = 1;
                placed = 1;
                break; // 首次适配，立即跳出
            }
        }

        // Step 4: 放置物品并更新可用区域
        if (placed) {
            // 获取最佳可用区域
            FreeRegion* best_region = &free_regions[best_region_idx];

            // 更新物品的放置信息
            item->x = best_region->x;
            item->y = best_region->y;
            item->rotated = best_rotated;

            // 更新容器高度（取当前物品底部的最大值）
            int item_bottom = item->y + best_h;
            if (item_bottom > bin_height) {
                bin_height = item_bottom;
            }

            // 分割可用区域：将当前区域替换为右侧和上方的剩余区域
            FreeRegion new_regions[2];
            int new_count = 0;

            // 右侧剩余区域（如果有）
            if (best_w < best_region->width) {
                new_regions[new_count++] = (FreeRegion){
                    best_region->x + best_w,
                    best_region->y,
                    best_region->width - best_w,
                    best_h
                };
            }

            // 上方剩余区域（如果有）
            if (best_h < best_region->height) {
                new_regions[new_count++] = (FreeRegion){
                    best_region->x,
                    best_region->y + best_h,
                    best_region->width,
                    best_region->height - best_h
                };
            }

            // 替换原可用区域为新区域：移除原区域，添加新区域
            free_regions[best_region_idx] = free_regions[free_count - 1]; // 用最后一个区域覆盖当前区域
            free_count--; // 减少区域数量

            // 添加新区域，动态扩展数组容量
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

            merge_free_regions(free_regions, &free_count);
        }
        else {
            // Step 5: 没有可用区域，在容器顶部新增区域
            int final_w, final_h, final_rotated;
            // 选择原始或旋转后能放入容器宽度的尺寸
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

            // 放置在容器顶部
            item->x = 0;
            item->y = bin_height;
            item->rotated = final_rotated;

            // 更新容器高度
            bin_height += final_h;

            // 添加右侧剩余区域（如果有）
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

    // 释放可用区域数组内存
    free(free_regions);

    // 返回容器最小高度
    return bin_height;
}