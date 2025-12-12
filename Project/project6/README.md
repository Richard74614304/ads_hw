# Texture Packing Problem
## Introduction
Texture Packing is to pack multiple rectangle shaped textures into one large texture. The resulting texture must have a given width and a minimum height.
This project requires you to design an approximation algorithm that runs in polynomial time. You must generate test cases of different sizes (from 10 to 10,000) with different distributions of widths and heights. A thorough analysis on all the factors that might affect the approximation ratio of your proposed algorithm is expected.

纹理打包是将多个矩形纹理打包到一个大纹理中。生成的纹理必须具有给定的宽度和最小的高度。
本项目要求您设计一个多项式时间运行的近似算法。您必须生成不同大小（从10到10,000）和不同宽度和高度分布的测试用例。需要对可能影响所提出算法近似比率的所有因素进行彻底分析。

## IO Formats
#### input format:
The first line contains two integers W and n, where W is the width of the resulting texture and n is the number of textures to be packed. The next n lines each contain two integers wi and hi, where wi and hi are the width and height of the i-th texture respectively.

第一行包含两个整数 W 和 n，其中 W 是生成的纹理的宽度，n 是要打包的纹理数量。接下来的 n 行中，每行包含两个整数 wi 和 hi，分别表示第 i 个纹理的宽度和高度。

#### output format:
The first line contains a single integer H, the height of the resulting texture. The next n lines each contain two integers xi and yi, where xi and yi are the coordinates of the bottom-left corner of the i-th texture in the resulting texture.

第一行包含一个整数 H，表示生成的纹理的高度。接下来的 n 行中，每行包含两个整数 xi 和 yi，分别表示第 i 个纹理在生成的纹理中的左下角坐标。

## Algorithms
- FFD-R (First Fit Decreasing - Revised)
- Skyline Algorithm

!!! Warning
    下面的内容来自豆老师，仅供参考，做报告的时候最好自己分析一下（我偷个懒）：

### 豆包生产：
#### 矩形装箱算法分析（FFD-R 与 Skyline 算法）

##### 1. 算法概述

本次实现了两种矩形装箱算法，均用于解决在固定宽度的容器中放置多个矩形物品，以最小化容器高度的问题，且均支持物品旋转（90度）。

- **FFD-R 算法**：基于首次适应递减（First Fit Decreasing）策略的改进算法，支持旋转
- **Skyline 算法**：一种经典的天际线装箱算法，通过维护天际线来优化放置位置选择

##### 2. 数据结构设计

###### 2.1 核心数据结构

```c
// 物品结构体
struct Item {
    int w;         // 宽度
    int h;         // 高度
    int x;         // 放置x坐标
    int y;         // 放置y坐标
    int rotated;   // 旋转标记（0:未旋转, 1:旋转90度）
};
typedef struct Item* Itemptr;
typedef struct Item* Items;
```

###### 2.2 专用数据结构

- **FFD-R 算法**：
```C
// 可用区域结构体：记录容器中未被占用的矩形区域
typedef struct FreeRegion {
    int x;         // 区域左上角x坐标
    int y;         // 区域左上角y坐标
    int width;     // 区域宽度
    int height;    // 区域高度
} FreeRegion;
```

- **Skyline 算法**：
```C
// 天际线节点结构体
typedef struct SkylineNode {
    int x;          // 横坐标
    int y;          // 该位置的高度
    struct SkylineNode* next;  // 下一个节点
} SkylineNode;

// 放置位置的代价结构体
typedef struct {
    int x;          // 放置的x坐标
    int y_base;     // 放置的基准高度
    int cost;       // 代价（越小越好）
} Placement;    
```

##### 3.算法流程
###### 3.1 FFD-R 算法流程
- 排序阶段：按预设标准（宽度、高度或面积）降序排序物品，可通过 SORT_STANDARD 宏定义选择排序标准（0: 宽度，1: 高度，2: 面积）
- 初始化：创建初始可用区域（整个容器宽度 W，高度为极大值），维护可用区域数组及数量
- 放置阶段：
对每个物品尝试两种状态（原始 / 旋转），遍历可用区域，采用首次适配策略找到第一个能容纳物品的区域；若找到合适区域，更新物品位置并分割可用区域；若未找到合适区域，在容器顶部新增区域放置物品
- 区域管理：
放置物品后分割可用区域为右侧和上方剩余区域，合并相邻的可用区域以减少冗余（水平和垂直方向）

###### 3.2 Skyline 算法流程
- 排序阶段：按物品面积降序排序（面积相同则按宽度降序）
- 初始化：
创建初始天际线（仅包含 (0,0) 节点）；维护天际线节点链表
- 放置阶段：
对每个物品，寻找最佳放置位置（考虑原始和旋转两种状态）；最佳位置判定标准：放置后高度最低（代价最小），x 坐标最小（辅助标准）
- 天际线更新：放置物品后，删除受影响的旧天际线节点；插入新的天际线节点并合并高度相同的相邻节点

##### 算法对比（？感觉有点问题）
| 特性 | FFD-R 算法 | Skyline 算法 |
|------|------------|--------------|
| 核心策略 | 首次适应递减，基于可用区域管理 | 维护天际线，选择最优放置位置 |
| 空间利用率 | 中等 | 较高 |
| 时间复杂度 | 较低（O(n²)） | 较高（O(n²)，但常数因子更大） |
| 实现复杂度 | 较简单 | 较复杂 |
| 旋转处理 | 尝试两种状态，取首次适配 | 比较两种状态代价，取最优 |
| 适用场景 | 对效率要求较高，中等利用率需求 | 对空间利用率要求较高，可接受较高计算成本 |

## Test Cases
Test cases are located in the `dataset` directory, mainly designed to cover scenarios that may be encountered in production. The test data includes various distributions such as UI textures, 3D textures, special effects textures, and mixed textures. Each category contains multiple test files of different sizes.

测试用例位于 `dataset` 目录中，主要设计思路是结合生产实际中可能遇到的情况。测试数据包括各种分布，如 UI 纹理、3D 纹理、特效纹理和混合纹理。每个类别包含多个不同大小的测试文件。

具体讲解一下各个数据集：
- UI 纹理集（小尺寸、多比例、大批量）
    - 背景: 手游 / 端游的 UI 资源（按钮、图标、文字背景、弹窗）通常是小尺寸（8×8~512×512）、比例多样（1:1/1:2/2:1/3:2 等）、数量大（数百 / 数千个），是纹理打包的高频场景。
    - 测试目的: 验证算法对 “小尺寸、多比例纹理” 的空间利用率，以及处理大批量纹理的稳定性。

- 3D 模型纹理（中大型、正方形为主、少数量）
    - 背景: 3D 模型的漫反射 / 法线 / 高光纹理通常是中大型尺寸（512×512~4096×4096）、正方形占比 90% 以上，数量较少（数十个），对空间利用率的要求极高（大纹理浪费 1px 都可能占用大量内存）。
    - 测试目的: 验证算法对 “大尺寸正方形纹理” 的紧凑放置能力，以及旋转（对正方形无意义）的冗余处理是否合理。

- 特效纹理（细长条、极端比例）
    - 背景: 游戏特效（火焰、流光、粒子）的纹理常是细长条、极端比例（如 1024×16、2048×32、16×1024），这类纹理容易导致空间碎片化，是算法的难点。
    - 测试目的: 验证算法对 “极端比例纹理” 的处理能力，以及区域合并优化是否能缓解碎片化。

- 混合尺寸纹理（真实项目的资源包快照）
    - 背景: 实际项目中，纹理图集常混合 UI、3D 模型、特效纹理，尺寸从 8×8 到 2048×2048 不等，比例杂乱。
    - 测试目的: 验证算法在真实复杂场景下的综合表现，是否满足项目的实际需求。

## compilation & execution
run `run.sh` to compile and execute the program on all test files.
(In Linux/macOS, ensure the script has execute permission: `chmod +x run.sh`)

The program is executed in Debian 13 environment.

## Testing Results
Testing results are stored in the `result` directory, organized by algorithm and test category. Each output file corresponds to an input test file, containing the packing height and coordinates of each texture.

测试结果存储在 `result` 目录中，按算法和测试类别进行组织。每个输出文件对应一个输入测试文件，包含打包高度和每个纹理的坐标。

下面是不同算法和不同测试集的部分结果汇总：

### FFD-R算法运行时间测试报告

| 数据集   | 测试文件               | 单次运行时间（秒） | 数据集平均运行时间（秒） |
|----------|------------------------|--------------------|--------------------------|
| **effect** | effect_textures1.txt   | 0.000013           | 0.0000146                |
|          | effect_textures2.txt   | 0.000012           |                          |
|          | effect_textures3.txt   | 0.000012           |                          |
|          | effect_textures4.txt   | 0.000011           |                          |
|          | effect_textures5.txt   | 0.000013           |                          |
|          | effect_textures6.txt   | 0.000012           |                          |
|          | effect_textures7.txt   | 0.000020           |                          |
|          | effect_textures8.txt   | 0.000020           |                          |
|          | effect_textures9.txt   | 0.000017           |                          |
|          | effect_textures10.txt  | 0.000016           |                          |
| **3D**    | 3D_textures1.txt       | 0.000005           | 0.0000067                |
|          | 3D_textures2.txt       | 0.000005           |                          |
|          | 3D_textures3.txt       | 0.000006           |                          |
|          | 3D_textures4.txt       | 0.000005           |                          |
|          | 3D_textures5.txt       | 0.000005           |                          |
|          | 3D_textures6.txt       | 0.000008           |                          |
|          | 3D_textures7.txt       | 0.000007           |                          |
|          | 3D_textures8.txt       | 0.000010           |                          |
|          | 3D_textures9.txt       | 0.000007           |                          |
|          | 3D_textures10.txt      | 0.000009           |                          |
| **mix**   | mix_textures1.txt      | 0.000257           | 0.000414                |
|          | mix_textures2.txt      | 0.000289           |                          |
|          | mix_textures3.txt      | 0.000274           |                          |
|          | mix_textures4.txt      | 0.000292           |                          |
|          | mix_textures5.txt      | 0.000361           |                          |
|          | mix_textures6.txt      | 0.000342           |                          |
|          | mix_textures7.txt      | 0.000592           |                          |
|          | mix_textures8.txt      | 0.000678           |                          |
|          | mix_textures9.txt      | 0.000661           |                          |
|          | mix_textures10.txt     | 0.000594           |                          |
| **UI**    | UI_textures1.txt       | 0.001353           | 0.001635                 |
|          | UI_textures2.txt       | 0.001581           |                          |
|          | UI_textures3.txt       | 0.001545           |                          |
|          | UI_textures4.txt       | 0.002203           |                          |
|          | UI_textures5.txt       | 0.002275           |                          |
|          | UI_textures6.txt       | 0.001652           |                          |
|          | UI_textures7.txt       | 0.000823           |                          |
|          | UI_textures8.txt       | 0.001861           |                          |
|          | UI_textures9.txt       | 0.001194           |                          |
|          | UI_textures10.txt      | 0.001864           |                          |

### Skyline算法运行时间测试报告
| 数据集   | 测试文件               | 单次运行时间（秒） | 数据集平均运行时间（秒） |
|----------|------------------------|--------------------|--------------------------|
| **effect** | effect_textures1.txt   | 0.000014           | 0.0000143                |
|          | effect_textures2.txt   | 0.000009           |                          |
|          | effect_textures3.txt   | 0.000020           |                          |
|          | effect_textures4.txt   | 0.000013           |                          |
|          | effect_textures5.txt   | 0.000014           |                          |
|          | effect_textures6.txt   | 0.000013           |                          |
|          | effect_textures7.txt   | 0.000015           |                          |
|          | effect_textures8.txt   | 0.000014           |                          |
|          | effect_textures9.txt   | 0.000016           |                          |
|          | effect_textures10.txt  | 0.000015           |                          |
| **3D**    | 3D_textures1.txt       | 0.000015           | 0.0000108                |
|          | 3D_textures2.txt       | 0.000013           |                          |
|          | 3D_textures3.txt       | 0.000011           |                          |
|          | 3D_textures4.txt       | 0.000014           |                          |
|          | 3D_textures5.txt       | 0.000013           |                          |
|          | 3D_textures6.txt       | 0.000011           |                          |
|          | 3D_textures7.txt       | 0.000011           |                          |
|          | 3D_textures8.txt       | 0.000010           |                          |
|          | 3D_textures9.txt       | 0.000005           |                          |
|          | 3D_textures10.txt      | 0.000005           |                          |
| **mix**   | mix_textures1.txt      | 0.000073           | 0.0000866                |
|          | mix_textures2.txt      | 0.000072           |                          |
|          | mix_textures3.txt      | 0.000076           |                          |
|          | mix_textures4.txt      | 0.000073           |                          |
|          | mix_textures5.txt      | 0.000076           |                          |
|          | mix_textures6.txt      | 0.000095           |                          |
|          | mix_textures7.txt      | 0.000093           |                          |
|          | mix_textures8.txt      | 0.000085           |                          |
|          | mix_textures9.txt      | 0.000115           |                          |
|          | mix_textures10.txt     | 0.000108           |                          |
| **UI**    | UI_textures1.txt       | 0.004827           | 0.0056144                |
|          | UI_textures2.txt       | 0.008877           |                          |
|          | UI_textures3.txt       | 0.009064           |                          |
|          | UI_textures4.txt       | 0.004962           |                          |
|          | UI_textures5.txt       | 0.005202           |                          |
|          | UI_textures6.txt       | 0.003842           |                          |
|          | UI_textures7.txt       | 0.004215           |                          |
|          | UI_textures8.txt       | 0.007774           |                          |
|          | UI_textures9.txt       | 0.006057           |                          |
|          | UI_textures10.txt      | 0.005324           |                          |

## Directory Structure
- `test.c`: Test program.
- `FFD-R.c`: Implementation of the FFD-R algorithm.
- `Skyline.c`: Implementation of the Skyline algorithm.
- `TP.h`: Header file containing common definitions and structures.
- `DataGen.c`: Test data generation program.
- `dataset/`: Contains test input files.
- `result/`: Contains output files generated by the program.
- `run.sh`: Script to compile and run the program on all test files.
- `gen.sh`: Script to generate test data.
- `README.md`: This readme file.
