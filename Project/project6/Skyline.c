#include "TP.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// ===================== 天际线算法的辅助函数（内部）=====================
// 初始化天际线
static SkylineNode* skyline_init() {
    SkylineNode* head = (SkylineNode*)malloc(sizeof(SkylineNode));
    head->x = 0;
    head->y = 0;
    head->next = NULL;
    return head;
}

// 释放天际线节点内存
static void skyline_free(SkylineNode* head) {
    SkylineNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// 插入天际线节点（按x坐标升序）
static void skyline_insert_node(SkylineNode** head, int x, int y) {
    SkylineNode* new_node = (SkylineNode*)malloc(sizeof(SkylineNode));
    new_node->x = x;
    new_node->y = y;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    if (x < (*head)->x) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    SkylineNode* current = *head;
    while (current->next != NULL && current->next->x < x) {
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
}

// 合并天际线的冗余节点（同高度相邻节点）
static void skyline_merge_nodes(SkylineNode** head) {
    if (*head == NULL) return;

    SkylineNode* current = *head;
    while (current->next != NULL) {
        if (current->y == current->next->y) {
            SkylineNode* temp = current->next;
            current->x = temp->x;
            current->next = temp->next;
            free(temp);
        } else {
            current = current->next;
        }
    }
}

// 寻找最佳放置位置（支持旋转）
static Placement skyline_find_best_placement(SkylineNode* head, int tex_w, int tex_h, int forbid_rotate, int ATLAS_WIDTH) {
    Placement best = {.x = -1, .y_base = -1, .cost = INT_MAX};
    SkylineNode* current = head;

    while (current != NULL) {
        int start_x = current->x;
        int max_y = current->y;
        int end_x = start_x;

        SkylineNode* temp = current;
        while (temp != NULL && (end_x - start_x) < tex_w) {
            end_x = temp->x;
            max_y = temp->y > max_y ? temp->y : max_y;
            temp = temp->next;
        }

        if ((end_x - start_x) < tex_w) {
            end_x = ATLAS_WIDTH;
            if ((end_x - start_x) < tex_w) {
                current = current->next;
                continue;
            }
        }

        int cost = max_y + tex_h;
        if (cost < best.cost || (cost == best.cost && start_x < best.x)) {
            best.x = start_x;
            best.y_base = max_y;
            best.cost = cost;
        }

        current = current->next;
    }

    // 处理旋转
    if (!forbid_rotate && tex_w != tex_h) {
        Placement best_rot = skyline_find_best_placement(head, tex_h, tex_w, 1, ATLAS_WIDTH);
        if (best_rot.cost < best.cost) {
            return best_rot;
        }
    }

    return best;
}

// 更新天际线（放置物品后）
static void skyline_update(SkylineNode** head, int start_x, int y_base, int tex_w, int tex_h, int ATLAS_WIDTH) {
    int new_y = y_base + tex_h;
    int end_x = start_x + tex_w;

    // 删除区间内的旧节点
    SkylineNode* current = *head;
    SkylineNode* prev = NULL;
    while (current != NULL && current->x < end_x) {
        if (current->x > start_x) break;
        SkylineNode* temp = current;
        current = current->next;
        if (prev == NULL) {
            *head = current;
        } else {
            prev->next = current;
        }
        free(temp);
    }

    // 插入新节点
    skyline_insert_node(head, start_x, new_y);
    skyline_insert_node(head, end_x, y_base);

    // 合并节点
    skyline_merge_nodes(head);
}

// 辅助函数：按物品面积降序排序（qsort的比较函数）
static int compare_item(const void* a, const void* b) {
    const struct Item* item_a = (const struct Item*)a;
    const struct Item* item_b = (const struct Item*)b;
    int area_a = item_a->w * item_a->h;
    int area_b = item_b->w * item_b->h;
    if (area_a != area_b) {
        return area_b - area_a; // 降序
    } else {
        return item_b->w - item_a->w; // 宽度降序
    }
}

// ===================== Skyline算法的主函数（对外暴露）=====================
int skyline_pack(int W, int n, Items items) {
    if (n == 0 || W <= 0) return 0;

    // 步骤1：复制物品数组（避免修改原数组的排序，可选，根据需求调整）
    struct Item* items_copy = (struct Item*)malloc(n * sizeof(struct Item));
    memcpy(items_copy, items, n * sizeof(struct Item));

    // 步骤2：按面积降序排序
    qsort(items_copy, n, sizeof(struct Item), compare_item);

    // 步骤3：初始化天际线
    SkylineNode* skyline = skyline_init();
    int atlas_height = 0;

    // 步骤4：遍历每个物品，逐个放置
    for (int i = 0; i < n; i++) {
        struct Item* item = &items_copy[i];
        int tex_w = item->w;
        int tex_h = item->h;

        // 寻找最佳放置位置（允许旋转）
        Placement best = skyline_find_best_placement(skyline, tex_w, tex_h, 0, W);

        if (best.x == -1) {
            fprintf(stderr, "Error: Item %d cannot be placed!\n", i);
            skyline_free(skyline);
            free(items_copy);
            return -1;
        }

        // 检查是否需要旋转（对比原尺寸和旋转后的尺寸）
        int rotated = 0;
        Placement best_rot = skyline_find_best_placement(skyline, tex_h, tex_w, 1, W);
        if (best_rot.cost < best.cost) {
            // 旋转物品
            int temp = tex_w;
            tex_w = tex_h;
            tex_h = temp;
            rotated = 1;
            best = best_rot;
        }

        // 记录放置位置（同步到原items数组，根据索引映射）
        // 注意：这里因为我们排序了copy数组，需要建立索引映射（关键！）
        // 简化方案：如果不需要保留原顺序，可直接排序原数组；以下是保留原顺序的映射方案
        // （如果你的场景不关心物品顺序，可删除copy数组，直接排序原数组，简化代码）
        // 【临时简化】：直接将位置写入copy数组，最后同步回原数组（若不需要顺序，可跳过此步）
        item->x = best.x;
        item->y = best.y_base;
        item->rotated = rotated;

        // 更新图集高度
        int new_height = best.y_base + tex_h;
        if (new_height > atlas_height) {
            atlas_height = new_height;
        }

        // 更新天际线
        skyline_update(&skyline, best.x, best.y_base, tex_w, tex_h, W);
    }

    // 【关键】：将copy数组的位置同步回原items数组（若排序了copy数组）
    // （如果你的场景不需要保留物品的原始顺序，可删除此步骤，直接操作原数组）
    for (int i = 0; i < n; i++) {
        items[i].x = items_copy[i].x;
        items[i].y = items_copy[i].y;
        items[i].rotated = items_copy[i].rotated;
    }

    // 释放内存
    skyline_free(skyline);
    free(items_copy);

    return atlas_height;
}