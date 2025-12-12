#include <stdio.h>
#include <stdlib.h>

struct Item {
    int w;
    int h;
    int x;
    int y;
    int rotated; // 0: 未旋转, 1: 旋转90度
};
typedef struct Item* Itemptr;
typedef struct Item* Items;

// 可用区域结构体：记录容器中未被占用的矩形区域
typedef struct FreeRegion {
    int x;      // 区域左上角x坐标
    int y;      // 区域左上角y坐标
    int width;  // 区域宽度
    int height; // 区域高度
} FreeRegion;

// 天际线节点结构体
typedef struct SkylineNode {
    int x;          // 横坐标
    int y;          // 该位置的高度
    struct SkylineNode* next;
} SkylineNode;

// 放置位置的代价结构体
typedef struct {
    int x;          // 放置的x坐标
    int y_base;     // 放置的基准高度
    int cost;       // 代价（越小越好）
} Placement;

int FFD_R(int W, int n, Items items);
int skyline_pack(int W, int n, Items items);