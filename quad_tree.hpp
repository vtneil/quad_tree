/*
Copyright (c) 2022 Vivatsathorn Thitasirivit

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

    // Definition of class QuadTree
    template<typename T, typename ConstructRect, typename Equal = std::equal_to<T>, typename Double = long double>
    class QuadTree {
    protected:
        // Maximum number of data per node before trying subdividing, default = 16
        static constexpr const size_t THRESHOLD = size_t(16);

        // Maximum depth of node level, default = 8
        static constexpr const size_t MAX_DEPTH = size_t(8);

        class TreeNode {
        public:
            std::unique_ptr<TreeNode> parent;
            std::array<std::unique_ptr<TreeNode>, 4> children;
            std::vector<T> data;
        };

        size_t m_size;
        Rect<Double> m_rect;
        std::unique_ptr<TreeNode> m_root;
        ConstructRect m_construct_rect;
        Equal m_equal;

    public:
        explicit QuadTree(const Rect<Double> &rect,
                          const ConstructRect &construct_rect = ConstructRect(),
                          const Equal &equal = Equal()) {
            m_size = 0;
            m_rect = rect;
            m_root = std::unique_ptr<TreeNode>(new TreeNode());
            m_construct_rect = construct_rect;
            m_equal = equal;
        }

    public:
        void insert(const T &value) {
            insert_value(m_root.get(), 0, m_rect, value);
        }

        void remove(const T &value) {
            remove_value(m_root.get(), m_rect, value);
        }

        Rect<Double> compute_rect(const Rect<Double> &rect, int quadrant) const {
            Vec2<Double> origin = rect.top_left();
            Vec2<Double> child = rect.shape() / 2;

            switch (quadrant) {
                case 0:
                    // Top left
                    return {origin, child};
                case 1:
                    // Top right
                    return {Vec2<Double>(origin.x + child.x, origin.y), child};
                case 2:
                    // Bottom left
                    return {Vec2<Double>(origin.x, origin.y + child.y), child};
                case 3:
                    // Bottom right
                    return {origin + child, child};
                default:
                    return {};
            }
        }

        int quadrant_of(const Rect<Double> &rect, const Rect<Double> &value) const {
            Vec2<Double> center = rect.center();
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

        void subdivide(TreeNode *node, const Rect<Double> &rect) {
            if (node == nullptr) return;
            if (!is_leaf(node)) return;

            for (auto &child: node->children) {
                child = std::unique_ptr<TreeNode>(new TreeNode());
            }

            std::vector<T> new_val{};

            for (const auto &value: node->data) {
                int q = quadrant_of(rect, ConstructRect(value));
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

        void insert_value(TreeNode *node, size_t depth, const Rect<Double> &rect, const T &value) {
            if (node == nullptr) return;
            if (!rect.contains(ConstructRect(value))) return;
            if (is_leaf(node)) {
                if (depth >= MAX_DEPTH || node->data.size() < THRESHOLD)
                    node->data.push_back(value);
                else {
                    subdivide(node, rect);
                    insert_value(node, depth, rect, value);
                }
            } else {
                int q = quadrant_of(rect, ConstructRect(value));
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

        bool remove(TreeNode *node, const Rect<Double> &rect, const T &value) {
            if (node == nullptr) return false;
            if (!rect.contains(ConstructRect(value))) return false;

            if (is_leaf(node)) {
                remove_value(node, value);
                return true;
            } else {
                int q = quadrant_of(rect, ConstructRect(value));
                if (q != -1) {
                    if (remove(node->children[q].get(), compute_rect(rect, q), value))
                        return try_merge(node);
                } else
                    remove_value(node, value);
                return false;
            }
        }
    };
}


#endif //QUAD_TREE_QUAD_TREE_HPP
