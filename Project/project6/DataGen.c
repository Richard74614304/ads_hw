#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ===================== 配置参数（可根据需求修改）=====================
#define ATLAS_WIDTH 2048          // 纹理图集宽度（W，手游常用2048，PC可用4096）
// 选择要生成的纹理类型（注释/取消注释对应行，仅保留一种或修改为混合数量）
// #define GENERATE_UI_TEXTURE       // UI纹理（小尺寸、多比例、大批量）
// #define GENERATE_MODEL_TEXTURE    // 3D模型纹理（中大型、正方形为主）
// #define GENERATE_EFFECT_TEXTURE   // 特效纹理（细长条、极端比例）
#define GENERATE_MIX_TEXTURE      // 混合纹理（综合场景）

// 各类纹理的数量（根据测试需求调整）
#ifdef GENERATE_UI_TEXTURE
    #define TEXTURE_COUNT 500      // UI纹理数量（n）
#elif defined(GENERATE_MODEL_TEXTURE)
    #define TEXTURE_COUNT 20       // 3D模型纹理数量
#elif defined(GENERATE_EFFECT_TEXTURE)
    #define TEXTURE_COUNT 30       // 特效纹理数量
#elif defined(GENERATE_MIX_TEXTURE)
    #define TEXTURE_COUNT 200      // 混合纹理数量
#endif

// 纹理打包常用的2的幂次尺寸数组（PoT，GPU采样效率高）
const int pot_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048};
const int pot_size_count = sizeof(pot_sizes) / sizeof(pot_sizes[0]);

// 纹理结构体（仅保留核心的宽高属性）
typedef struct {
    int w;                // 宽度
    int h;                // 高度
} Texture;

// ===================== 辅助函数 =====================
/**
 * 获取随机的2的幂次尺寸（不超过max_size）
 * @param max_size 最大尺寸限制
 * @return 随机PoT尺寸
 */
int get_rand_pot_size(int max_size) {
    int idx;
    do {
        idx = rand() % pot_size_count;
    } while (pot_sizes[idx] > max_size); // 确保不超过最大尺寸
    return pot_sizes[idx];
}

// ===================== 生成不同类型纹理的函数 =====================
/**
 * 生成UI纹理（小尺寸、多比例、大批量）
 * @return 纹理数组（需手动释放）
 */
Texture* generate_ui_textures() {
    Texture *textures = (Texture*)malloc(TEXTURE_COUNT * sizeof(Texture));
    if (!textures) {
        perror("Failed to allocate UI textures");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        int w = get_rand_pot_size(512); // UI纹理最大宽度512
        int h = get_rand_pot_size(512); // UI纹理最大高度512

        // 随机比例：30%正方形、30%宽>高、30%高>宽、10%特殊比例（非PoT，模拟非标UI）
        int ratio_type = rand() % 10;
        if (ratio_type < 3) {
            // 正方形（w=h）
            h = w;
        } else if (ratio_type < 6) {
            // 宽>高（h = w / 2）
            h = w / 2;
            if (h < 8) h = 8; // 最小尺寸8
        } else if (ratio_type < 9) {
            // 高>宽（w = h / 2）
            w = h / 2;
            if (w < 8) w = 8;
        } else {
            // 特殊比例（如3:2，用16的倍数模拟非标UI）
            w = (rand() % 6 + 2) * 16; // 32~128
            h = (rand() % 4 + 2) * 16; // 32~96
        }

        textures[i].w = w;
        textures[i].h = h;
    }

    return textures;
}

/**
 * 生成3D模型纹理（中大型、正方形为主）
 * @return 纹理数组（需手动释放）
 */
Texture* generate_model_textures() {
    Texture *textures = (Texture*)malloc(TEXTURE_COUNT * sizeof(Texture));
    if (!textures) {
        perror("Failed to allocate model textures");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        // 90%为正方形（512/1024/2048），10%为非正方形（如1024×512）
        if (i < TEXTURE_COUNT * 0.9) {
            // 正方形
            textures[i].w = get_rand_pot_size(2048);
            textures[i].h = textures[i].w;
        } else {
            // 非正方形（宽是高的2倍或反之）
            textures[i].w = get_rand_pot_size(2048);
            textures[i].h = textures[i].w / 2;
            if (textures[i].h < 512) textures[i].h = 512; // 最小512
        }
    }

    return textures;
}

/**
 * 生成特效纹理（细长条、极端比例）
 * @return 纹理数组（需手动释放）
 */
Texture* generate_effect_textures() {
    Texture *textures = (Texture*)malloc(TEXTURE_COUNT * sizeof(Texture));
    if (!textures) {
        perror("Failed to allocate effect textures");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        // 前半部分：水平细长条（宽大、高小），后半部分：垂直细长条（高大、宽小）
        if (i < TEXTURE_COUNT / 2) {
            // 水平细长条：宽=1024/2048，高=8/16/32
            textures[i].w = get_rand_pot_size(ATLAS_WIDTH);
            textures[i].h = pot_sizes[rand() % 3]; // 8、16、32
        } else {
            // 垂直细长条：宽=8/16/32，高=1024/2048
            textures[i].w = pot_sizes[rand() % 3]; // 8、16、32
            textures[i].h = get_rand_pot_size(ATLAS_WIDTH);
        }
    }

    return textures;
}

/**
 * 生成混合纹理（UI+3D+特效的混合，模拟真实项目）
 * @return 纹理数组（需手动释放）
 */
Texture* generate_mix_textures() {
    Texture *textures = (Texture*)malloc(TEXTURE_COUNT * sizeof(Texture));
    if (!textures) {
        perror("Failed to allocate mix textures");
        exit(EXIT_FAILURE);
    }

    int ui_count = TEXTURE_COUNT * 0.5;    // 50% UI纹理
    int model_count = TEXTURE_COUNT * 0.1; // 10% 3D模型纹理
    int effect_count = TEXTURE_COUNT * 0.05; // 5% 特效纹理
    int random_count = TEXTURE_COUNT - ui_count - model_count - effect_count; // 35% 随机纹理

    // 1. 填充UI纹理（小尺寸、多比例）
    for (int i = 0; i < ui_count; i++) {
        textures[i].w = get_rand_pot_size(512);
        textures[i].h = get_rand_pot_size(512);
        // 随机调整比例（增加多样性）
        if (rand() % 2 == 0) {
            textures[i].h = textures[i].w / 2;
            if (textures[i].h < 8) textures[i].h = 8;
        }
    }

    // 2. 填充3D模型纹理（中大型、正方形）
    int idx = ui_count;
    for (int i = 0; i < model_count; i++, idx++) {
        textures[idx].w = get_rand_pot_size(1024);
        textures[idx].h = textures[idx].w;
    }

    // 3. 填充特效纹理（细长条）
    for (int i = 0; i < effect_count; i++, idx++) {
        textures[idx].w = (rand() % 2 == 0) ? get_rand_pot_size(ATLAS_WIDTH) : pot_sizes[rand() % 3];
        textures[idx].h = (textures[idx].w > 512) ? pot_sizes[rand() % 3] : get_rand_pot_size(ATLAS_WIDTH);
    }

    // 4. 填充随机纹理（非标准比例，模拟真实项目的杂项）
    for (int i = 0; i < random_count; i++, idx++) {
        textures[idx].w = get_rand_pot_size(1024);
        textures[idx].h = get_rand_pot_size(1024);
    }

    return textures;
}

// ===================== 主函数 =====================
int main() {
    // 初始化随机数种子（确保每次运行生成不同数据）
    srand((unsigned int)time(NULL));

    // 生成纹理数据（根据选择的类型调用对应函数）
    Texture *textures = NULL;
    #ifdef GENERATE_UI_TEXTURE
        textures = generate_ui_textures();
    #elif defined(GENERATE_MODEL_TEXTURE)
        textures = generate_model_textures();
    #elif defined(GENERATE_EFFECT_TEXTURE)
        textures = generate_effect_textures();
    #elif defined(GENERATE_MIX_TEXTURE)
        textures = generate_mix_textures();
    #endif

    // 打开文件（w模式：覆盖原有内容，不存在则创建）
    FILE *fp = fopen("./dataset/mix/mix_textures.txt", "w");
    if (!fp) {
        perror("Failed to open ./dataset/mix/mix_textures.txt");
        free(textures);
        return EXIT_FAILURE;
    }

    // 第一行：输出图集宽度W和纹理数量n
    fprintf(fp, "%d %d\n", ATLAS_WIDTH, TEXTURE_COUNT);

    // 后续n行：输出每个纹理的w和h
    for (int i = 0; i < TEXTURE_COUNT; i++) {
        fprintf(fp, "%d %d\n", textures[i].w, textures[i].h);
    }

    // 释放内存+关闭文件
    free(textures);
    fclose(fp);

    printf("Testdata has been successfully generated to ./dataset/UI/UI_textures.txt!\n");
    printf("width W:%d, number of textures n: %d\n", ATLAS_WIDTH, TEXTURE_COUNT);

    return 0;
}