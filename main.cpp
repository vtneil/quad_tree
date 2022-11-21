#include "quad_tree.h"
#include "ref.cpp"
#include <iostream>
#include <vector>

using self::Point;
using self::TreeNode;
using self::QuadTree;
using std::cout;
using std::cin;

int main() {
    Point tl{0, 10};
    Point br{10, 0};
    bool f;
    int v = 5;
    char c = 'b';

    QuadTree<char> t{tl, br};
    cout << t.mid_point().x << ", " << t.mid_point().y << '\n';
    t.insert(new TreeNode<char>(Point(1, 10), c));
    t.insert(new TreeNode<char>(Point(9, 9), 'b'));
    t.insert(new TreeNode<char>(Point(15, 15), 'c'));
    t.insert(new TreeNode<char>(Point(7, 3), 'd'));

    t.insert(new TreeNode<char>(Point(15, 15), 'c'));
    t.insert(new TreeNode<char>(Point(7, 3), 'x'));

    auto k = t.find(Point(7, 3));
    f = k.second;
    auto g = f ? k.first->data : 'z';


    cout << g << '\n' << f << '\n';
    cout << "Size = " << t.size() << '\n';

//    t.clear();
//    t.insert(new TreeNode<char>(Point(7, 3), 'd'));

    k = t.find(Point(1, 10));
    f = k.second;
    g = f ? k.first->data : 'z';
    cout << g << '\n' << f << '\n';
    cout << "Size = " << t.size() << '\n';

    return 0;
}