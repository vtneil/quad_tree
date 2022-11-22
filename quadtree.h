#ifndef QUAD_TREE_QUADTREE_H
#define QUAD_TREE_QUADTREE_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <algorithm>
#include <iostream>

#include "vec2.h"
#include "qtnode.h"

#define BOT_LEFT 0
#define TOP_LEFT 1
#define BOT_RIGHT 2
#define TOP_RIGHT 3

namespace qt {
    enum enclosure {
        OUT_OF_BOUND, PARTIAL_BOUND, IN_BOUND
    };

    template<typename T, typename PairT = std::pair<Vertex, T>, typename ContainerT = std::vector<PairT>>
    class QuadTree {
    private:
        typedef QuadTreeNode<T, PairT, ContainerT> Node;

        class PairComp {
        public:
            bool operator()(const PairT &lhs, const PairT &rhs) const {
                return lhs.first < rhs.first;
            }
        };

        QuadTreeNode<T> *m_root;
        PairComp m_pair_comp;
        unsigned max_depth;
        unsigned max_bucket_size;
        bool m_sort;

    public:
        explicit QuadTree<T, PairT, ContainerT>(Vertex center = Vertex{0, 0},
                                                Vertex range = Vertex{1, 1},
                                                unsigned bucket_size = 1,
                                                unsigned depth = 16,
                                                bool sort = false);

        ~QuadTree();

        Node *&root() {
            return m_root;
        }

        bool insert(const Vertex &point, const T &data);

        bool update(const Vertex &point, const T &data);

        bool contains(const Vertex &point);

        bool remove(const Vertex &point);

        std::vector<std::pair<Vertex, T>> data_in_region(const Vertex &bottom_left, const Vertex &top_right);

    private:
        Node *&child_node(const Vertex &v, Node *&node);

        Vertex new_center(int direction, Node *node);

        int direction(const Vertex &point, Node *node);

        bool insert(const Vertex &v, const T &data, Node *&node, unsigned depth);

        void reduce(std::stack<Node *> &nodes);

        void add_points_to_result(Node *node, std::vector<std::pair<Vertex, T>> &results);

        bool in_region(const Vertex &point, const Vertex &bottom_left, const Vertex &top_right);

        enclosure status(const Vertex &center, const Vertex &range, const Vertex &bottom_left, const Vertex &top_right);

        void print_nodes(Node *&node, unsigned int depth = 0) const {
            // Print this node's address
            for (unsigned int i = 0; i < depth; ++i) std::cout << "|   ";
            std::cout << "|  At depth = " << depth << ", Node at address " << &node << " has parent "
                      << &(node->parent);
            if (node->leaf) std::cout << " (Leaf node)\n";
            else std::cout << " (Stem node)\n";

            // Print data in the bucket
            for (auto const &data: node->bucket) {
                for (unsigned int i = 0; i < depth; ++i) std::cout << "|   ";
                std::cout << "[  <*> Point " << data.first << " has data = " << data.second << '\n';
            }

            // Print children addresses
            if (!node->leaf) {
                for (unsigned int i = 0; i < depth; ++i) std::cout << "|   ";
                std::cout << "|  This node has valid children: \n";
            }
            int c = 0;
            for (Node *&child: node->children) {
                if (child != nullptr) {
                    for (unsigned int i = 0; i < depth; ++i) std::cout << "|   ";
                    std::cout << "|  -> Child #" << c;
                    // Recursively print nodes and corresponding data if child is not null.
                    std::cout << '\n';
                    print_nodes(child, 1 + depth);
                }
                ++c;
            }
        }

        void print_data(Node *&node) {
            for (auto const &data: node->bucket)
                std::cout << "<*> Point " << data.first << " has data = " << data.second << '\n';

            for (Node *&child: node->children)
                if (child != nullptr)
                    print_data(child);
        }

    public:
        void print_preorder() {
            std::cout << "Parent node is currently broken (not valid)!\n\n";
            std::cout << "Tree root is at address " << &m_root << '\n';
            print_nodes(m_root, 0);
            std::cout << '\n';
        }

        void print_data() {
            print_data(m_root);
            std::cout << '\n';
        }
    };
}

// Class member functions definition file
#include "quadtree.cpp"

#endif //QUAD_TREE_QUADTREE_H
