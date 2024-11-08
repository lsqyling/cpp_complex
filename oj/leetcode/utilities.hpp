//
// Created by 10580.
//

#ifndef CPP_COMPLEX_OJ_LEETCODE_UTILITIES_HPP
#define CPP_COMPLEX_OJ_LEETCODE_UTILITIES_HPP
#include <iostream>
#include <utility>
#include <algorithm>
#include <random>
#include <format>

inline int gen_random(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 engine{rd()};
    static std::uniform_int_distribution<int> dist(min, max);

    return dist(engine);
}

inline std::vector<int> gen_rd_array(int n, int min, int max)
{
    std::vector<int> arr(n);
    for (int i = 0; i < n; ++i)
    {
        arr[i] = gen_random(min, max);
    }

    return arr;
}

inline bool is_sorted(const std::vector<int> &arr)
{
    int n = static_cast<int>(arr.size());
    for (int i = 0; i < n - 1; ++i)
    {
        if (arr[i] > arr[i+1])
            return false;
    }
    return true;
}


#endif //CPP_COMPLEX_OJ_LEETCODE_UTILITIES_HPP
