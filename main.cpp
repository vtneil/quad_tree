#include "quadtree.h"
#include "vec2.h"
#include <iostream>

using std::cout;
using std::cin;

using namespace qt;

int main() {
    Vertex origin{0, 0};
    Vertex radius{10000, 10000};
    unsigned bucket_size = 128; // default = 8
    unsigned depth = 16; // default = 16
    bool sort_points_in_node = false;

    QuadTree<int> tree{origin, radius, bucket_size, depth, sort_points_in_node};

    for (int i = -1000; i < 1000; ++i) {
        tree.insert(Vertex(i + 50, 400 - i), 2 * i + 9);
//         Remove test.
//        tree.remove(Vertex(i + 50, 400 - i));
    }

//    tree.insert(Vertex(-1, -1), 0);
//    tree.insert(Vertex(-1, 1), 1);
//    tree.insert(Vertex(1, -1), 2);
//    tree.insert(Vertex(1, 1), 3);

    // Update and insert test.

//    tree.update(Vertex(59, 391), 80);
//    tree.insert(Vertex(59, 391), 50);

    tree.print_data();
//    tree.print_preorder();

    return 0;
}