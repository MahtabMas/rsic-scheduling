#pragma once
#include <cmath>
#include <functional>

// Define a small epsilon for floating-point comparison
constexpr double EPSILON = 1e-10; 

inline bool isZero(double value) {
    return std::fabs(value) < EPSILON;
}

// Define a custom hash function for std::pair<int64_t, int64_t>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};