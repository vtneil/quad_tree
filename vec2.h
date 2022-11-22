#ifndef QUAD_TREE_VEC2_H
#define QUAD_TREE_VEC2_H

#include <cstdint>
#include <cstddef>
#include <vector>
#include <stack>
#include <queue>
#include <string>
#include <algorithm>
#include <iostream>

namespace qt {
    template<typename T>
    class Vec2 {
    public:
        T x;
        T y;

        Vec2(T x, T y) : x{x}, y{y} {}

        Vec2(const Vec2<T> &other) {
            x = other.x;
            y = other.y;
        }

        explicit Vec2(const std::pair<T, T> &other) {
            x = other.first;
            y = other.second;
        }

        Vec2() : Vec2(0, 0) {}

        Vec2<T> &operator=(Vec2<T> other) {
            std::swap(x, other.x);
            std::swap(y, other.y);
            return *this;
        }

        Vec2<T> &operator=(std::pair<T, T> other) {
            std::swap(x, other.first);
            std::swap(y, other.second);
            return *this;
        }

        Vec2<T> &operator+=(const Vec2<T> &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        Vec2<T> &operator+=(const std::pair<T, T> &other) {
            x += other.first;
            y += other.second;
            return *this;
        }

        Vec2<T> &operator-=(const Vec2<T> &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        Vec2<T> &operator-=(const std::pair<T, T> &other) {
            x -= other.first;
            y -= other.second;
            return *this;
        }

        Vec2<T> &operator*=(const Vec2<T> &other) {
            x *= other.x;
            y *= other.y;
            return *this;
        }

        Vec2<T> &operator*=(const std::pair<T, T> &other) {
            x *= other.first;
            y *= other.second;
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

        Vec2<T> &operator/=(const std::pair<T, T> &other) {
            x /= other.first;
            y /= other.second;
            return *this;
        }

        Vec2<T> &operator/=(T t) {
            x /= t;
            y /= t;
            return *this;
        }

        Vec2<T> operator+(const Vec2<T> &other) const {
            Vec2<T> tmp(*this);
            tmp += other;
            return tmp;
        }

        Vec2<T> operator+(const std::pair<T, T> &other) const {
            Vec2<T> tmp(*this);
            tmp += other;
            return tmp;
        }

        Vec2<T> operator-(const Vec2<T> &other) const {
            Vec2<T> tmp(*this);
            tmp -= other;
            return tmp;
        }

        Vec2<T> operator-(const std::pair<T, T> &other) const {
            Vec2<T> tmp(*this);
            tmp -= other;
            return tmp;
        }

        Vec2<T> operator*(const Vec2<T> &other) const {
            Vec2<T> tmp(*this);
            tmp *= other;
            return tmp;
        }

        Vec2<T> operator*(const std::pair<T, T> &other) const {
            Vec2<T> tmp(*this);
            tmp *= other;
            return tmp;
        }

        Vec2<T> operator*(T t) const {
            Vec2<T> tmp(*this);
            tmp *= t;
            return tmp;
        }

        Vec2<T> operator/(const Vec2<T> &other) const {
            Vec2<T> tmp(*this);
            tmp /= other;
            return tmp;
        }

        Vec2<T> operator/(const std::pair<T, T> &other) const {
            Vec2<T> tmp(*this);
            tmp /= other;
            return tmp;
        }

        Vec2<T> operator/(T t) const {
            Vec2<T> tmp(*this);
            tmp /= t;
            return tmp;
        }

        bool operator==(const Vec2<T> &other) const {
            return x == other.x && y == other.y;
        }

        bool operator==(const std::pair<T, T> &other) const {
            return x == other.first && y == other.second;
        }

        bool operator<(const Vec2<T> &other) const {
            if (x < other.x) return true;
            if (x == other.x && y < other.y) return true;
            return false;
        }

        friend std::ostream &operator<<(std::ostream &os, const Vec2<T> &vec2) {
            os << '(' << vec2.x << ", " << vec2.y << ')';
            return os;
        }
    };

    typedef Vec2<long double> Vertex;
}

#endif //QUAD_TREE_VEC2_H
