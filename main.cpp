//#include "refs/quad_tree_v0.h"
//#include "refs/quad_tree.hpp"
//#include "refs/ref.cpp"
#include "quadtree.h"
#include "vec2.h"
#include <iostream>
#include <vector>

using std::cout;
using std::cin;

using namespace qt;

int main() {
    Vertex origin{0, 0};
    Vertex rad{1000, 1000};

    QuadTree<int> tree{origin, rad, 2};

    for (int i = -100; i < 100; ++i) {
        tree.insert(Vertex(i + 50, 400 - i), 2 * i);
    }

    tree.print_preorder();

    return 0;
}