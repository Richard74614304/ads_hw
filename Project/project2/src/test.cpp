#include <stdio.h>
#include <stdlib.h>
#include "ds.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <windows.h> 

int main(int argc, char* argv[]) {
    const char* filename = argv[1];
    graph G = readGraphFromFile(filename);
    if (!G) {
        printf("Error: Failed to read graph data\n");
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    // 只显示前后20组结果
    Dijkstra(G, 1);

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end_time - start_time;
    printf("Dijkstra algorithm execution time: %.6f seconds\n", duration.count());

    freegraph(G, G->nv);
    return 0;
}