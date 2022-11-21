//#include "quad_tree_0.h"
#include "quad_tree.hpp"
#include "ref.cpp"
#include <iostream>
#include <vector>

using self::Vec2;
using std::cout;
using std::cin;

int main() {
    Vec2<int> pa{-10, 10};
    Vec2<int> pb{10, -10};
    int a = 5;

    auto c = pa / 3;

    cout << c.x << ' ' << c.y;

    return 0;
}