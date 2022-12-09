#include "quadtree.h"
#include "vec2.h"
#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>

#define GRID_SIZE 100

using std::cout;
using std::cin;

using namespace qt;

//int benchmark(QuadTree<int> *&tree, int arg1, int arg2) {
//    static int count = 0;
//    clock_t start = clock();
//    // START
//
//    tree->insert(Vertex(arg1, arg2), 0);
//
//    // STOP
//    clock_t stop = clock();
//    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
//    printf("%d\t", count);
//    count++;
//    printf("%.5f\n", elapsed);
//    return 0;
//}

int main() {
    Vertex origin{0, 0};
    Vertex radius{GRID_SIZE, GRID_SIZE};
    unsigned bucket_size = 128; // default = 8
    unsigned depth = 16; // default = 16
    bool sort_points_in_node = false;

    QuadTree<int> *tree;
    tree = new QuadTree<int>{origin, radius, bucket_size, depth, sort_points_in_node};

    for (int i = 1; i < GRID_SIZE + 1; ++i) {
        for (int j = 1; j < GRID_SIZE + 1; ++j) {
            tree->insert(Vertex(i, j), i + j);
        }
    }

    std::cout << *(tree->at(Vertex(12, 5)));

    return 0;
}