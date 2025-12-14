#include "TP.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// ===================== skyline helper functions ======================
// initialize the skyline linked list
static SkylineNode* skyline_init() {
    SkylineNode* head = (SkylineNode*)malloc(sizeof(SkylineNode));
    head->x = 0;
    head->y = 0;
    head->next = NULL;
    return head;
}

// free the skyline linked list
static void skyline_free(SkylineNode* head) {
    SkylineNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// insert a new skyline node in sorted order
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

// merge adjacent skyline nodes with the same height
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

// find the best placement for an item (rotation considered)
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

    // handle rotation
    if (!forbid_rotate && tex_w != tex_h) {
        Placement best_rot = skyline_find_best_placement(head, tex_h, tex_w, 1, ATLAS_WIDTH);
        if (best_rot.cost < best.cost) {
            return best_rot;
        }
    }

    return best;
}

// update skyline after placing an item
static void skyline_update(SkylineNode** head, int start_x, int y_base, int tex_w, int tex_h, int ATLAS_WIDTH) {
    int new_y = y_base + tex_h;
    int end_x = start_x + tex_w;

    // delete covered nodes
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

    // insert new nodes
    skyline_insert_node(head, start_x, new_y);
    skyline_insert_node(head, end_x, y_base);

    // merge nodes
    skyline_merge_nodes(head);
}

// function to compare items by area (for sorting)
static int compare_item(const void* a, const void* b) {
    const struct Item* item_a = (const struct Item*)a;
    const struct Item* item_b = (const struct Item*)b;
    int area_a = item_a->w * item_a->h;
    int area_b = item_b->w * item_b->h;
    if (area_a != area_b) {
        return area_b - area_a; 
    } else {
        return item_b->w - item_a->w;
    }
}

// ===================== Skyline main function =====================
int skyline_pack(int W, int n, Items items) {
    if (n == 0 || W <= 0) return 0;

    // Step 1: Create a copy of items to sort
    struct Item* items_copy = (struct Item*)malloc(n * sizeof(struct Item));
    memcpy(items_copy, items, n * sizeof(struct Item));

    // Step 2: Sort items by area (descending)
    qsort(items_copy, n, sizeof(struct Item), compare_item);

    // Step 3: Initialize the skyline
    SkylineNode* skyline = skyline_init();
    int atlas_height = 0;

    // Step 4: Traverse each item and place it
    for (int i = 0; i < n; i++) {
        struct Item* item = &items_copy[i];
        int tex_w = item->w;
        int tex_h = item->h;

        // find the best placement (rotation considered)
        Placement best = skyline_find_best_placement(skyline, tex_w, tex_h, 0, W);

        if (best.x == -1) {
            fprintf(stderr, "Error: Item %d cannot be placed!\n", i);
            skyline_free(skyline);
            free(items_copy);
            return -1;
        }

        // check if rotation is better
        int rotated = 0;
        Placement best_rot = skyline_find_best_placement(skyline, tex_h, tex_w, 1, W);
        if (best_rot.cost < best.cost) {
            // rotation
            int temp = tex_w;
            tex_w = tex_h;
            tex_h = temp;
            rotated = 1;
            best = best_rot;
        }

        // update item placement info
        item->x = best.x;
        item->y = best.y_base;
        item->rotated = rotated;

        // update atlas height
        int new_height = best.y_base + tex_h;
        if (new_height > atlas_height) {
            atlas_height = new_height;
        }

        // update the skyline
        skyline_update(&skyline, best.x, best.y_base, tex_w, tex_h, W);
    }

    // Step 5: Synchronize the final positions back to the original items array
    for (int i = 0; i < n; i++) {
        items[i].x = items_copy[i].x;
        items[i].y = items_copy[i].y;
        items[i].rotated = items_copy[i].rotated;
    }

    // Free resources
    skyline_free(skyline);
    free(items_copy);

    return atlas_height;
}