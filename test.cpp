#include "quadtree.h"
#include "vec2.h"
#include <iostream>

#define GRID_SIZE 10

using std::cout;
using std::cin;

using namespace qt;

typedef int DATA_TYPE;
constexpr unsigned BUCKET_SIZE = 4; // default = 8
constexpr unsigned MAX_DEPTH = 16; // default = 16
Vertex ORIGIN{0, 0};
Vertex RADIUS{GRID_SIZE, GRID_SIZE};
constexpr bool SORT_BUCKET = false;

int main() {
    QuadTree<DATA_TYPE> *tree;
    tree = new QuadTree<DATA_TYPE>{ORIGIN, RADIUS, BUCKET_SIZE, MAX_DEPTH, SORT_BUCKET};

    QuadTree<DATA_TYPE>::viterator it(tree->root());
    auto it1 = tree->insert({1, 1}, 50).first;
    tree->insert({2, 2}, 40);
    tree->insert({3, 3}, 30);
    tree->insert({4, 4}, 20);
    it1 = tree->insert({5, 5}, 10).first;

    for (auto &x: *it1) {
        std::cout << x.first << ' ' << x.second << ' ';
    }

    std::cout << '\n';

    tree->print_preorder();

    int ret;
    std::cin >> ret;

    return 0;
}