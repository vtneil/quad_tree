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
#include <cstdio>

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

    using enclosure::OUT_OF_BOUND;
    using enclosure::PARTIAL_BOUND;
    using enclosure::IN_BOUND;

    template<typename T, typename PairT = std::pair<Vertex, T>, typename ContainerT = std::vector<PairT>>
    class QuadTree {
    private:
        typedef QuadTreeNode<T, PairT, ContainerT> Node;

        class PairComp;

        class TreeNodeIterator;

        class TreeIterator;

        QuadTreeNode<T> *m_root;
        PairComp m_pair_comp;
        unsigned max_depth;
        unsigned max_bucket_size;
        bool m_sort;
        size_t m_size;

    public:
        typedef TreeNodeIterator viterator;
        typedef TreeIterator iterator;

        explicit QuadTree<T, PairT, ContainerT>(Vertex center = Vertex{0, 0},
                                                Vertex range = Vertex{1, 1},
                                                unsigned bucket_size = 1,
                                                unsigned depth = 16,
                                                bool sort = false);

        ~QuadTree();

        Node *&root() {
            return m_root;
        }

        size_t size() const {
            return m_size;
        }

        bool insert(const Vertex &point, const T &data);

        bool update(const Vertex &point, const T &data);

        bool contains(const Vertex &point);

        bool remove(const Vertex &point);

        std::vector<std::pair<Vertex, T>> data_in_region(const Vertex &bottom_left, const Vertex &top_right);

        std::vector<std::pair<Vertex, T>> extract_all();

        viterator vbegin();

        viterator vend();

        iterator begin();

        iterator end();

        void data_in_subtrees(Node *node);

    private:
        Node *&child_node(const Vertex &v, Node *&node);

        Vertex new_center(int direction, Node *node);

        int direction(const Vertex &point, Node *node);

        bool insert(const Vertex &v, const T &data, Node *&node, QuadTree::Node *parent_node, unsigned depth);

        void reduce(std::stack<Node *> &nodes);

        void add_points_to_result(Node *node, std::vector<std::pair<Vertex, T>> &results);

        bool in_region(const Vertex &point, const Vertex &bottom_left, const Vertex &top_right);

        enclosure status(const Vertex &center, const Vertex &range, const Vertex &bottom_left, const Vertex &top_right);

        void print_nodes(Node *&node, unsigned int depth = 0);

        void print_data(Node *&node);

        void traverse(Node *node, std::queue<Node *> &nodes);

    public:
        void print_preorder() {
            std::cout << "Tree size is " << m_size << '\n';
            printf("Tree root is at address %p\n", m_root);
            print_nodes(m_root, 0);
            printf("\n");
        }

        void print_data() {
            print_data(m_root);
            viterator it{m_root};
            ++it;
            printf("\n");
        }
    };

    template<typename T, typename PairT, typename ContainerT>
    class QuadTree<T, PairT, ContainerT>::PairComp {
    public:
        bool operator()(const PairT &lhs, const PairT &rhs) const {
            return lhs.first < rhs.first;
        }
    };

    template<typename T, typename PairT, typename ContainerT>
    class QuadTree<T, PairT, ContainerT>::TreeNodeIterator {
        friend class QuadTree;

    protected:
        Node *node;
    public:
        TreeNodeIterator() : node(nullptr) {}

        explicit TreeNodeIterator(Node *node) : node{node} {}

        TreeNodeIterator &operator++() {
            // Use stack tree traversal method here.
            return *this;
        }

        TreeNodeIterator &operator--() {
            // Use stack tree traversal method here.
            return *this;
        }

        TreeNodeIterator operator++(int) {
            TreeNodeIterator tmp(this);
            operator++();
            return tmp;
        }

        TreeNodeIterator operator--(int) {
            TreeNodeIterator tmp(this);
            operator--();
            return tmp;
        }

        ContainerT &operator*() {
            return node->m_bucket;
        }

        ContainerT *operator->() {
            return &(node->m_bucket);
        }

        bool operator==(const TreeNodeIterator &other) const {
            return node == other.node;
        }

        bool operator!=(const TreeNodeIterator &other) const {
            return node != other.node;
        }
    };

    // Must fix all of this below!!!
    template<typename T, typename PairT, typename ContainerT>
    class QuadTree<T, PairT, ContainerT>::TreeIterator {
        friend class QuadTree;

    protected:
        Node *node;
    public:
        TreeIterator() : node(nullptr) {}

        explicit TreeIterator(Node *node) : node{node} {}

        TreeIterator &operator++() {
            // Use stack tree traversal method here.
            return *this;
        }

        TreeIterator &operator--() {
            // Use stack tree traversal method here.
            return *this;
        }

        TreeIterator operator++(int) {
            TreeIterator tmp(this);
            operator++();
            return tmp;
        }

        TreeIterator operator--(int) {
            TreeIterator tmp(this);
            operator--();
            return tmp;
        }

        PairT &operator*() {
            return PairT();
        }

        PairT *operator->() {
            return &(new PairT());
        }

        bool operator==(const TreeIterator &other) const {
            return node->m_bucket == other.node->m_bucket;
        }

        bool operator!=(const TreeIterator &other) const {
            return node != other.node;
        }
    };
}

// Class member functions definition file
#include "quadtree.cpp"

#endif //QUAD_TREE_QUADTREE_H
