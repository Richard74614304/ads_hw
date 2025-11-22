#include <stdio.h>
#include <stdlib.h>

int main(){
    double C, D, dpg;
    int n;
    scanf("%lf %lf %lf %d", &C, &D, &dpg, &n);

    double* d = (double*) malloc((n + 1) * sizeof(double));
    double* p = (double*) malloc((n + 1) * sizeof(double));

    for (int i = 0; i < n; i++) {
        scanf("%lf %lf", &p[i], &d[i]);
    }

    // bubble_sort
    d[n] = D;
    p[n] = 0;
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= n - i; j++) {
            if (d[j] > d[ j + 1]) {
                double tempd = d[j];
                d[j] = d[j + 1];
                d[j + 1] = tempd;
                double tempp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = tempp;
            }
        }
    }

    // dp(failed)
    /*
    double minprice;
    int isable = 1;
    double maxdist = 0;

    double* dp = (double*) malloc((n + 1) * sizeof(double));
    double* g = (double*) malloc((n + 1) * sizeof(double));
    for (int i = 0; i <= n ; i++) {
        dp[i] = (i == 0) ? 0 : dp[i - 1] + (d[i] - d[i - 1]) / dpg * p[i - 1];
        if (i != 0 && d[i] - d[i - 1] > C * dpg) {
            maxdist = d[i - 1];
            isable = 0;
            break;
        }
        for(int j = 0; j < i - 1; j++) {
            double temp = dp[j] + (d[i] - d[j]) / dpg * p[j];
            if (temp < dp[i]) dp[i] = temp;
        }
    }
    */
    
    // greedy(succeed!)
    double g = 0;
    double maxdist = 0;
    int isable = 1;
    double cost = 0;
    double maxrange = C * dpg;

    if (d[0] != 0) {
        printf("The maximum travel distance = %.2lf", 0);
        return 0;
    }

    for (int i = 0; i < n; ) {
        int next = -1;
        double min_price = 1e9;
        int min_index = -1;
        int found_cheaper = 0;
    
        for (int j = i + 1; j <= n; j++) {
            double distance = d[j] - d[i];
        
            if (distance > maxrange) break;
        
            if (p[j] < p[i]) {
                next = j;
                found_cheaper = 1;
                break;
            }
        
            if (p[j] < min_price) {
                min_price = p[j];
                min_index = j;
            }
        }
    
        if (!found_cheaper && min_index != -1) {
            next = min_index;
        }
    
        if (next == -1) {
            isable = 0;
            maxdist = d[i] + maxrange;
            break;
        }
    
        double distance_to_next = d[next] - d[i];
        double need_g = distance_to_next / dpg;
    
        if (p[next] >= p[i]) {
            cost += (C - g) * p[i];
            g = C;
        } else {
            double delta_g = need_g - g;
            if (delta_g > 0) {
                cost += delta_g * p[i];
                g = need_g;
            }
        }
    
        g -= need_g;
        i = next;
    
        if (i == n) break;
    }

    if (!isable) {
        printf("The maximum travel distance = %.2lf", maxdist);
    }
    else {
        printf("%.2lf", cost);
    }

    free(d);
    free(p);
    return 0;
}