#ifndef QUAD_TREE_LIBRARY_H
#define QUAD_TREE_LIBRARY_H

#include <cstdint>
#include <cstddef>
#include <algorithm>

constexpr long double SUBDIVIDE_MIN_SIZE = 1;

namespace self {
    struct Point {
        long double x;
        long double y;

        Point(long double x, long double y) : x{x}, y{y} {}

        Point(Point const &other) {
            x = other.x;
            y = other.y;
        }

        Point() : Point(0, 0) {}

        Point &operator=(Point other) {
            std::swap(x, other.x);
            std::swap(y, other.y);
            return *this;
        }
    };

    template<typename T>
    struct TreeNode {
        Point pos;
        T data;

        TreeNode(const Point &pos, const T &data) : pos{pos}, data{data} {}

        TreeNode(const TreeNode &other) {
            pos = other.pos;
            data = other.data;
        }

        TreeNode() : pos{Point{}}, data{T{}} {}

        TreeNode &operator=(TreeNode other) {
            std::swap(pos, other.pos);
            std::swap(data, other.data);
            return *this;
        }
    };

    template<typename T>
    class QuadTree {
        class TreeIterator {
        protected:
            TreeNode<T> *ptr;

        public:
            TreeIterator() : ptr{nullptr} {}

            TreeIterator(const TreeIterator &other) : ptr{other.ptr} {}

            explicit TreeIterator(TreeNode<T> *node) : ptr{node} {}

            TreeIterator &operator++() {
                return {};
            }

            TreeIterator &operator--() {
                return {};
            }

            TreeIterator operator++(int) {
                TreeIterator tmp{*this};
                operator++();
                return tmp;
            }

            TreeIterator operator--(int) {
                TreeIterator tmp{*this};
                operator--();
                return tmp;
            }

            T &operator*() { return ptr->data; }

            T *operator->() { return *(ptr->data); }

            bool operator==(const TreeIterator &other) { return ptr == other.ptr; }

            bool operator!=(const TreeIterator &other) { return ptr != other.ptr; }
        };

    public:
        typedef TreeIterator iterator;

    public: // change to protected
        size_t m_size;
        Point m_top_left;
        Point m_bottom_right;

        TreeNode<T> *m_node;

        QuadTree<T> *m_tree_top_left;
        QuadTree<T> *m_tree_top_right;
        QuadTree<T> *m_tree_bottom_left;
        QuadTree<T> *m_tree_bottom_right;
        QuadTree<T> *m_tree_parent;

    public:
        QuadTree(const Point &top_left, const Point &bottom_right) : m_top_left{top_left},
                                                                     m_bottom_right{bottom_right} {
            m_size = 0;
            m_node = nullptr;
            m_tree_top_left = nullptr;
            m_tree_top_right = nullptr;
            m_tree_bottom_left = nullptr;
            m_tree_bottom_right = nullptr;
            m_tree_parent = nullptr;
        }

        QuadTree(const QuadTree<T> &other) {
            m_size = other.m_size;
            m_top_left = other.m_top_left;
            m_bottom_right = other.m_bottom_right;
            m_node = other.m_node;
            m_tree_top_left = other.m_tree_top_left;
            m_tree_top_right = other.m_tree_top_right;
            m_tree_bottom_left = other.m_tree_bottom_left;
            m_tree_bottom_right = other.m_tree_bottom_right;
            m_tree_parent = other.m_tree_parent;
        }

        QuadTree() {
            m_size = 0;
            m_top_left = {0.0, 1.0};
            m_bottom_right = {1.0, 0.0};
            m_node = nullptr;
            m_tree_top_left = nullptr;
            m_tree_top_right = nullptr;
            m_tree_bottom_left = nullptr;
            m_tree_bottom_right = nullptr;
            m_tree_parent = nullptr;
        }

        QuadTree<T> &operator=(QuadTree<T> other) {
            std::swap(m_size, other.m_size);
            std::swap(m_top_left, other.m_top_left);
            std::swap(m_bottom_right, other.m_bottom_right);
            std::swap(m_node, other.m_node);
            std::swap(m_tree_top_left, other.m_tree_top_left);
            std::swap(m_tree_top_right, other.m_tree_top_right);
            std::swap(m_tree_bottom_left, other.m_tree_bottom_left);
            std::swap(m_tree_bottom_right, other.m_tree_bottom_right);
            std::swap(m_tree_parent, other.m_tree_parent);
        }

        ~QuadTree() {
            clear();
        }

    protected:
        bool insert_node(TreeNode<T> *);

        bool erase_node(TreeNode<T> *);

        TreeNode<T> *find_node(const Point &);

    public:
        size_t size() {
            return m_size;
        }

        std::pair<TreeNode<T> *, bool> find(const Point &point) {
            TreeNode<T> *node = find_node(point);
            std::pair<TreeNode<T> *, bool> ret{node, node != nullptr};
            return ret;
        }

        bool erase(TreeNode<T> *node) {
            return erase_node(node);
        }

        bool insert(TreeNode<T> *node) {
            return insert_node(node);
        }

        Point mid_point() {
            return {(m_top_left.x + m_bottom_right.x) / 2, (m_top_left.y + m_bottom_right.y) / 2};
        }

        long double width() {
            return abs(m_top_left.x - m_bottom_right.x);
        }

        long double height() {
            return abs(m_top_left.y - m_bottom_right.y);
        }

        std::pair<long double, long double> shape() {
            return {width(), height()};
        }

        bool in_bound(const Point &point) {
            return (point.x >= m_top_left.x && point.x <= m_bottom_right.x
                    && point.y <= m_top_left.y && point.y >= m_bottom_right.y);
        }

        void clear() {
            delete_all_trees(m_tree_top_left);
            delete_all_trees(m_tree_top_right);
            delete_all_trees(m_tree_bottom_left);
            delete_all_trees(m_tree_bottom_right);

            m_tree_parent = nullptr;
            m_size = 0;
            m_node = nullptr;
            m_tree_top_left = nullptr;
            m_tree_top_right = nullptr;
            m_tree_bottom_left = nullptr;
            m_tree_bottom_right = nullptr;
        }

        QuadTree<T> *copy(QuadTree<T> *, QuadTree<T> *);

        void delete_all_trees(QuadTree<T> *tree);
    };

    template<typename T>
    bool QuadTree<T>::insert_node(TreeNode<T> *node) {
        // Invalid node
        if (node == nullptr) return false;

        // Out of bound check
        if (!in_bound(node->pos)) return false;

        // Can't perform subdividing anymore (min size) (Insert)
        if (width() <= SUBDIVIDE_MIN_SIZE && height() <= SUBDIVIDE_MIN_SIZE) {
            // Node doesn't exist, then insert (Don't insert if already exists)
            if (m_node == nullptr) {
                m_node = node;
                return true;
            }
            m_node = node;
            return false;
        }

        Point middle = this->mid_point();

        // Left half or Right half
        if (node->pos.x <= middle.x) {
            // Top left or bottom left
            if (node->pos.y >= middle.y) {
                if (m_tree_top_left == nullptr)
                    m_tree_top_left = new QuadTree<T>(Point(m_top_left), Point(middle));
                ++m_size;
                return m_tree_top_left->insert_node(node);
            } else {
                if (m_tree_bottom_left == nullptr)
                    m_tree_bottom_left = new QuadTree<T>(Point(m_top_left.x, middle.y),
                                                         Point(middle.x, m_bottom_right.y));
                ++m_size;
                return m_tree_bottom_left->insert_node(node);
            }
        } else {
            // Top right or bottom right
            if (node->pos.y >= mid_point().y) {
                if (m_tree_top_right == nullptr)
                    m_tree_top_right = new QuadTree<T>(Point(middle.x, m_top_left.y),
                                                       Point(m_bottom_right.x, middle.y));
                ++m_size;
                return m_tree_top_right->insert_node(node);
            } else {
                if (m_tree_bottom_right == nullptr)
                    m_tree_bottom_right = new QuadTree<T>(Point(middle),
                                                          Point(m_bottom_right));
                ++m_size;
                return m_tree_bottom_right->insert_node(node);
            }
        }
    }

    template<typename T>
    TreeNode<T> *QuadTree<T>::find_node(const Point &point) {
        // Out of bound check
        if (!in_bound(point)) return nullptr;

        // Can't perform subdividing anymore (min size) (Divide search)
        if (m_node != nullptr) return m_node;

        Point middle = this->mid_point();

        // Left half or Right half
        if (point.x <= middle.x) {
            // Top left or bottom left
            if (point.y >= middle.y) {
                if (m_tree_top_left == nullptr)
                    return nullptr;
                return m_tree_top_left->find_node(point);
            } else {
                if (m_tree_bottom_left == nullptr)
                    return nullptr;
                return m_tree_bottom_left->find_node(point);
            }
        } else {
            // Top right or bottom right
            if (point.y >= middle.y) {
                if (m_tree_top_right == nullptr)
                    return nullptr;
                return m_tree_top_right->find_node(point);
            } else {
                if (m_tree_bottom_right == nullptr)
                    return nullptr;
                return m_tree_bottom_right->find_node(point);
            }
        }
    }

    template<typename T>
    bool QuadTree<T>::erase_node(TreeNode<T> *) {
        return false;
    }

    template<typename T>
    QuadTree<T> *QuadTree<T>::copy(QuadTree<T> *src, QuadTree<T> *parent) {
        return {};
    }

    template<typename T>
    void QuadTree<T>::delete_all_trees(QuadTree<T> *tree) {
        if (tree == nullptr) return;
        delete_all_trees(tree->m_tree_top_left);
        delete_all_trees(tree->m_tree_top_right);
        delete_all_trees(tree->m_tree_bottom_left);
        delete_all_trees(tree->m_tree_bottom_right);

        delete tree;
//        delete m_node;
    }
}

#endif //QUAD_TREE_LIBRARY_H
