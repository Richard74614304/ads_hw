#include <stdio.h>
#include <stdlib.h>

struct Item {
    int w;
    int h;
    int x;
    int y;
    int rotated; // 0: unrotated, 1: rotated 90 degrees
};
typedef struct Item* Itemptr;
typedef struct Item* Items;

// Free region structure: available space in the container
typedef struct FreeRegion {
    int x;      // x coordinate of the region
    int y;      // y coordinate of the region
    int width;  // width of the region
    int height; // height of the region
} FreeRegion;

// Skyline node structure
typedef struct SkylineNode {
    int x;          // x coordinate
    int y;          // height at this position
    struct SkylineNode* next;
} SkylineNode;

// Placement structure for skyline algorithm
typedef struct {
    int x;          // x coordinate
    int y_base;     // base height of the placement
    int cost;       // cost (lower is better)
} Placement;

int FFD_R(int W, int n, Items items);
int skyline_pack(int W, int n, Items items);