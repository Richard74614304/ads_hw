#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// ==================== 配置参数（修改此处切换输入规模）====================
#define N 100000
#define D_VAL 1000000000LL

// 商品价格和优惠券的取值范围（确保c_i < p_i）
#define MIN_PRICE 2          // 商品最低价格（至少2，保证c_i可取值）
#define MAX_PRICE 1000000000 // 商品最高价格（1e9）

int main() {
    // 1. 初始化随机种子（保证每次生成不同数据，若需固定数据可替换为定值如srand(123456)）
    srand((unsigned int)time(NULL));

    // 2. 打开输出文件（覆盖写入模式）
    FILE *fp = fopen("worstcase.txt", "w");
    if (fp == NULL) {
        perror("Failed to open testdata.txt");
        return 1;
    }

    // 3. 写入N和D
    fprintf(fp, "%d %lld\n", N, D_VAL);

    // 4. 生成并写入商品价格数组（p_i ≥ MIN_PRICE）
    for (int i = 0; i < N; i++) {
        // 生成MIN_PRICE到MAX_PRICE之间的随机价格
        int price = MIN_PRICE + rand() % (MAX_PRICE - MIN_PRICE + 1);
        fprintf(fp, "%d ", price);
        //fprintf(fp, "5 ");
    }
    fprintf(fp, "\n");

    // 5. 生成并写入优惠券数组（c_i < p_i，需先读取价格再生成？不，这里边生成价格边存，再生成优惠券）
    // 注：若N很大，为节省内存，可分两次循环（先写价格到文件，再重新读取价格生成优惠券），但小规模可直接存数组
    int prices[N];
    // 重新生成价格并存入数组（避免重复计算，也可优化为一次循环）
    for (int i = 0; i < N; i++) {
        prices[i] = MIN_PRICE + rand() % (MAX_PRICE - MIN_PRICE + 1);
    }
    // 生成优惠券（c_i ∈ [0, prices[i]-1]）
    for (int i = 0; i < N; i++) {
        int coupon = rand() % prices[i];  // 0到prices[i]-1的随机数
        fprintf(fp, "%d ", coupon);
        //fprintf(fp, "4 ");
    }
    fprintf(fp, "\n");

    // 6. 关闭文件
    fclose(fp);
    printf("Testdata has been stored in testdata.txt, (N=%d, D=%lld)\n", N, D_VAL);

    return 0;
}