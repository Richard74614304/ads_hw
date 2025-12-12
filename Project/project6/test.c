#include <stdio.h>
#include <stdlib.h>
#include "TP.h"
#include <sys/time.h>

int main(int argc, char** argv) {
    // file reading
    if (argc != 3) { fprintf(stderr, "Usage: %s <testfile> <resultfile>\n", argv[0]); return 1; }
    FILE *fp1 = fopen(argv[1], "r");
    if (!fp1) { perror("fopen"); return 1; }

    int W, n;
    fscanf(fp1, "%d %d", &W, &n);
    Items items = (Items)malloc(n * sizeof(struct Item));
    for(int i = 0; i < n; i++) {
        fscanf(fp1, "%d %d", &items[i].w, &items[i].h);
    }
    
    int result = -1;

    // Timing start
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // call the approximation algorithm
    // result = FFD_R(W, n, items);
    result = skyline_pack(W, n, items);
    
    // Timing end
    gettimeofday(&end, NULL);
    double time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;

    // print the result
    printf("The width of the container: %d\n", W);
    printf("Minimum height on running %s: %d\n", argv[1], result);
    printf("Time: %.6f seconds\n", time);

    FILE *fp2 = fopen(argv[2], "w");
    if (fp2 == NULL) { // 检查文件是否成功打开
        perror("Failed to open result file");
        free(items); // 打开失败也要释放内存
        return 1;
    }

    // 2. 将结果写入文件（替换原printf为fprintf，第一个参数为文件指针fp）
    fprintf(fp2, "===== Skyline Algorithm Result =====\n");
    fprintf(fp2, "Container Width: %d\n", W);
    fprintf(fp2, "Minimum Height: %d\n", result);
    fprintf(fp2, "Item Placement:\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp2 , "Item %d: (original w=%d, h=%d) → placed at (x=%d, y=%d), rotated=%s\n",
               i+1, items[i].w, items[i].h,
               items[i].x, items[i].y,
               items[i].rotated ? "Yes" : "No");
    }
    fclose(fp2);

    free(items);
    return 0;
}