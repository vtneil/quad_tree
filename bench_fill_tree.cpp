#include "quadtree.h"
#include "vec2.h"
#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>

#define GRID_SIZE 400

using std::cout;
using std::cin;

using namespace qt;

void fill_tree(QuadTree<int> &tree, int grid_size) {
    for (int i = -grid_size; i < grid_size; ++i) {
        for (int j = -grid_size; j < grid_size; ++j) {
            tree.insert(Vertex(i, j), 2 * i + 9);
        }
    }
}

int benchmark(QuadTree<int> &tree, int arg) {
    clock_t start = clock();
    // START

    fill_tree(tree, arg);

    // STOP
    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("%d\t", arg * arg);
    printf("%.5f\n", elapsed);
    return 0;
}

int main() {
    Vertex origin{0, 0};
    Vertex radius{GRID_SIZE, GRID_SIZE};
    unsigned bucket_size = 128; // default = 8
    unsigned depth = 16; // default = 16
    bool sort_points_in_node = false;

    QuadTree<int> *tree;

    for (int i = 1; i < 61; i += 10) {
        tree = new QuadTree<int>{origin,
                                 {static_cast<long double>(i), static_cast<long double>(i)},
                                 bucket_size, depth, sort_points_in_node};
        benchmark(*tree, i);

        delete tree;
    }

    return 0;
}