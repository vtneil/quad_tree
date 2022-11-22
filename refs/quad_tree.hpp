#ifndef QUAD_TREE_QUAD_TREE_HPP
#define QUAD_TREE_QUAD_TREE_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <algorithm>

namespace self {
    // Definition of class Vec2
    template<typename T>
    class Vec2 {
    public:
        T x;
        T y;

        Vec2(T x, T y) : x{x}, y{y} {}

        Vec2(Vec2 const &other) {
            x = other.x;
            y = other.y;
        }

        Vec2() : Vec2(0, 0) {}

        Vec2<T> &operator=(Vec2 other) {
            std::swap(x, other.x);
            std::swap(y, other.y);
            return *this;
        }

        Vec2<T> &operator+=(const Vec2<T> &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vec2<T> &operator-=(const Vec2<T> &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vec2<T> &operator*=(const Vec2<T> &other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        Vec2<T> &operator*=(T t) {
            x *= t;
            y *= t;
            return *this;
        }

        Vec2<T> &operator/=(const Vec2<T> &other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }

        Vec2<T> &operator/=(T t) {
            x /= t;
            y /= t;
            return *this;
        }

        Vec2<T> &operator+(const Vec2<T> &other) {
            *this += other;
            return *this;
        }

        Vec2<T> &operator-(const Vec2<T> &other) {
            *this -= other;
            return *this;
        }

        Vec2<T> &operator*(const Vec2<T> &other) {
            *this *= other;
            return *this;
        }

        Vec2<T> &operator*(T t) {
            *this *= t;
            return *this;
        }

        Vec2<T> &operator/(const Vec2<T> &other) {
            *this /= other;
            return *this;
        }

        Vec2<T> &operator/(T t) {
            *this /= t;
            return *this;
        }
    };

    // Definition of class Rect
    template<typename T>
    class Rect {
    protected:
        T m_left;
        T m_top;
        T m_width;
        T m_height;

    public:
        Rect(T left, T top, T width, T height) {
            set_origin(left, top);
            set_dimension(width, height);
        }

        Rect(const Vec2<T> &pos, const Vec2<T> &shape) {
            set_origin(pos);
            set_dimension(shape);
        }

        Rect(Rect const &other) {
            m_left = other.m_left;
            m_top = other.m_top;
            m_width = other.m_width;
            m_height = other.m_height;
        }

        Rect() : Rect(0, 0, 0, 0) {}

        Rect<T> &operator=(Rect other) {
            std::swap(m_left, other.m_left);
            std::swap(m_top, other.m_top);
            std::swap(m_width, other.m_width);
            std::swap(m_height, other.m_height);
            return *this;
        }

        bool contains(const Rect<T> &other) const {
            return left() <= other.left() && other.right() <= right() &&
                   top() <= other.top() && other.bottom() <= bottom();
        }

        bool contains(const Vec2<T> &point) const {
            return left() <= point.x && point.x <= right() &&
                   top() <= point.y && point.y <= bottom();
        }

        bool intersects(const Rect<T> &other) const {
            return !(left() >= other.right() || right() <= other.left() ||
                     top() >= other.bottom() || bottom() <= other.top());
        }

        T top() const {
            return m_top;
        }

        T bottom() const {
            return m_top + m_height;
        }

        T left() const {
            return m_left;
        }

        T right() const {
            return m_left + m_width;
        }

        T width() const {
            return m_width;
        }

        T height() const {
            return m_height;
        }

        Vec2<T> top_left() const {
            return {left(), top()};
        }

        Vec2<T> top_right() const {
            return {right(), top()};
        }

        Vec2<T> bottom_left() const {
            return {left(), bottom()};
        }

        Vec2<T> bottom_right() const {
            return {right(), bottom()};
        }

        Vec2<T> center() const {
            return {m_left + m_width / 2, m_top + m_height / 2};
        }

        Vec2<T> shape() const {
            return {m_width, m_height};
        }

        void set_origin(T left, T top) {
            m_left = left;
            m_top = top;
        }

        void set_origin(const Vec2<T> &pos) {
            m_left = pos.x;
            m_top = pos.y;
        }

        void set_dimension(T width, T height) {
            m_width = abs(width);
            m_height = abs(height);
        }

        void set_dimension(const Vec2<T> &shape) {
            m_width = abs(shape.x);
            m_height = abs(shape.y);
        }
    };

    // Definition of class Node
    template<typename T, typename RectT>
    class Node {
    public:
        RectT rect;
        T data;
        size_t id;

        Node(size_t id, const T &data) : data{data}, rect{RectT()}, id{id} {}

        Node() : data{T()}, rect{RectT()}, id{0} {}

        bool operator==(const Node &other) const {
            return id == other.id && data == other.data;
        }
    };

    // Definition of class RetrieveRect
    template<typename NodeT, typename DoubleT>
    class RetrieveRect {
    public:
        Rect<DoubleT> operator()(const NodeT &node) const {
            return node.rect;
        }
    };

    // Definition of class QuadTreeSimple
    template<typename T, typename ConstructRect, typename Equal = std::equal_to<T>, typename DoubleT = long double>
    class QuadTreeSimple {
    protected:
        // Maximum number of data per node before trying subdividing, default = 16
        static constexpr const size_t THRESHOLD = size_t(16);

        // Maximum depth of node level, default = 8
        static constexpr const size_t MAX_DEPTH = size_t(8);

        // Definition of class TreeNode
        class TreeNode {
        public:
            std::unique_ptr<TreeNode> parent;
            std::array<std::unique_ptr<TreeNode>, 4> children;
            std::vector<T> data;
        };

        size_t m_size;
        Rect<DoubleT> m_rect;
        std::unique_ptr<TreeNode> m_root;
        ConstructRect m_construct_rect;
        Equal m_equal;

    public:
        explicit QuadTreeSimple(const Rect<DoubleT> &rect,
                                const ConstructRect &construct_rect,
                                const Equal &equal = Equal()) {
            m_size = 0;
            m_rect = rect;
            m_root = std::unique_ptr<TreeNode>(new TreeNode());
            m_construct_rect = construct_rect;
            m_equal = equal;
        }

    public:
        size_t size() {
            return m_size;
        }

        std::vector<T> find_rect_intersections(const Rect<DoubleT> &rect) {
            std::vector<T> ret{};
            find_rect_intersections(m_root.get(), m_rect, rect, ret);
            return ret;
        }

        void insert(const T &value) {
            insert_value(m_root.get(), 0, m_rect, value);
        }

        bool erase(const T &value) {
            return remove(m_root.get(), m_rect, value);
        }

        Rect<DoubleT> compute_rect(const Rect<DoubleT> &rect, int quadrant) const {
            Vec2<DoubleT> origin = rect.top_left();
            Vec2<DoubleT> child = rect.shape() / 2;

            switch (quadrant) {
                case 0:
                    // Top left
                    return {origin, child};
                case 1:
                    // Top right
                    return {Vec2<DoubleT>(origin.x + child.x, origin.y), child};
                case 2:
                    // Bottom left
                    return {Vec2<DoubleT>(origin.x, origin.y + child.y), child};
                case 3:
                    // Bottom right
                    return {origin + child, child};
                default:
                    return {};
            }
        }

        int quadrant_of(const Rect<DoubleT> &rect, const Rect<DoubleT> &value) const {
            Vec2<DoubleT> center = rect.center();
            if (value.right() < center.x) {
                // Top Left
                if (value.bottom() < center.y)
                    return 0;
                    // Bottom left
                else if (value.top() >= center.y)
                    return 2;
                // Out of bound
                return -1;
            } else if (value.left() >= center.x) {
                // Top Right
                if (value.bottom() < center.y)
                    return 1;
                    // Bottom Right
                else if (value.top() >= center.y)
                    return 3;
                // Out of bound
                return -1;
            }
            // Out of every bound;
            return -1;
        }

    private:
        bool is_leaf(const TreeNode *node) const {
            return !static_cast<bool>(node->children[0]);
        }

        void subdivide(TreeNode *node, const Rect<DoubleT> &rect) {
            if (node == nullptr) return;
            if (!is_leaf(node)) return;

            for (std::unique_ptr<TreeNode> &child: node->children) {
                child = std::unique_ptr<TreeNode>(new TreeNode());
            }

            std::vector<T> new_val{};

            for (const T &value: node->data) {
                int q = quadrant_of(rect, m_construct_rect(value));
                if (q != -1)
                    node->children[q]->data.push_back(value);
                else
                    new_val.push_back(value);
            }

            // Use and destroy
            node->data = std::move(new_val);
        }

        bool try_merge(TreeNode *node) {
            if (node == nullptr) return false;
            if (is_leaf(node)) return false; // (Leaf node can't be merged.)

            size_t nb = node->data.size();
            for (const std::unique_ptr<TreeNode> &child: node->children) {
                if (!is_leaf(child.get()))
                    return false;
                nb += child->data.size();
            }
            if (nb > THRESHOLD) return false;

            node->data.reserve(nb);
            for (const std::unique_ptr<TreeNode> &child: node->children) {
                node->data.insert(node->data.end(), child->data.begin(), child->data.end());
            }
            for (std::unique_ptr<TreeNode> &child: node->children) {
                child.reset();
            }
            return true;
        }

        void insert_value(TreeNode *node, size_t depth, const Rect<DoubleT> &rect, const T &value) {
            if (node == nullptr) return;
            if (!rect.contains(m_construct_rect(value))) return;
            if (is_leaf(node)) {
                if (depth >= MAX_DEPTH || node->data.size() < THRESHOLD)
                    node->data.push_back(value);
                else {
                    subdivide(node, rect);
                    insert_value(node, depth, rect, value);
                }
            } else {
                int q = quadrant_of(rect, m_construct_rect(value));
                if (q != -1)
                    insert_value(node->children[q].get(),
                                 1 + depth,
                                 compute_rect(rect, q),
                                 value);
                else
                    node->data.push_back(value);
            }
        }

        void remove_value(TreeNode *node, const T &value) {
            auto comp = [this, &value](const T &rhs) {
                return this->m_equal(value, rhs);
            };
            auto it = std::find_if(node->data.begin(), node->data.end(), comp);
            if (it == node->data.end()) return;

            // Move to the back and pop it off.
            *it = std::move(node->data.back());
            node->data.pop_back();
        }

        bool remove(TreeNode *node, const Rect<DoubleT> &rect, const T &value) {
            if (node == nullptr) return false;
            if (!rect.contains(m_construct_rect(value))) return false;

            if (is_leaf(node)) {
                remove_value(node, value);
                return true;
            } else {
                int q = quadrant_of(rect, m_construct_rect(value));
                if (q != -1) {
                    if (remove(node->children[q].get(), compute_rect(rect, q), value))
                        return try_merge(node);
                } else
                    remove_value(node, value);
                return false;
            }
        }

        void find_rect_intersections(TreeNode *node, const Rect<DoubleT> &rect, const Rect<DoubleT> &query,
                                     std::vector<T> &ret) const {
            if (node == nullptr) return;
            if (!query.intersects(rect)) return;

            for (const T &value: node->data)
                if (query.intersects(m_construct_rect(value)))
                    ret.push_back(value);

            if (!is_leaf(node)) {
                for (int i = 0; i < node->children.size(); ++i) {
                    Rect<DoubleT> child_rect = compute_rect(rect, i);
                    if (query.intersects(child_rect))
                        find_rect_intersections(node->children[i].get(), child_rect, query, ret);
                }
            }
        }
    };
}


#endif //QUAD_TREE_QUAD_TREE_HPP
