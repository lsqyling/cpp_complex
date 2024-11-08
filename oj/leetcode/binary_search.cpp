#include "utilities.hpp"

// find x in arr[L, R], returns the pos of x, otherwise returns -1;
int find(const std::vector<int> &arr, int x)
{
    int l = 0, r = arr.size() - 1, m;
    while (l <= r)
    {
        m = l + ((r - l) >> 1);
        if (x < arr[m])
            r = m - 1;
        else if (arr[m] < x)
            l = m + 1;
        else
            return m;
    }
    return -1;
}


//  find >= x in arr[L, R], returns the pos, otherwise -1 instead.
int find_left(const std::vector<int> &arr, int x)
{
    int l = 0, r = arr.size() - 1, m;
    int pos = -1;
    while (l <= r)
    {
        m = l + (r - l) / 2;
        if (x <= arr[m])
        {
            pos = m;
            r = m - 1;
        }
        else
            l = m + 1;
    }
    return pos;
}

//  find <= x in arr[L, R], returns the rightmost pos, otherwise -1 instead.
int find_right(const std::vector<int> &arr, int x)
{
    int l = 0, r = arr.size() - 1, m;
    int pos = -1;
    while (l <= r)
    {
        m = l + (r - l) / 2;
        if (arr[m] <= x)
        {
            pos = m;
            l = m + 1;
        }
        else
            r = m - 1;
    }
    return pos;
}

void test_finds()
{
    std::vector<int> arr{0, 3, 4, 6, 8, 12, 15, 21, 21, 21, 23, 25};
    std::cout << std::format("25 pos: {}\n", find(arr, 23));

    std::cout << std::format(">=16 left pos: {}\n", find_left(arr, 16));
    std::cout << std::format("<=21 right pos: {}\n", find_right(arr, 21));
}

namespace leetcode_162 {
//162. Find Peak Element
class Solution
{
public:
    int findPeakElement(std::vector<int> &nums)
    {
        int n = nums.size();
        if (n == 1)
            return 0;
        if (nums[0] > nums[1])
            return 0;
        if (nums[n-1] > nums[n-2])
            return n - 1;

        int l = 0, r = n - 1, m;
        while (l <= r)
        {
            m = l + (r - l) / 2;
            if (nums[m+1] > nums[m])
                l = m;
            else if (nums[m-1] > nums[m])
                r = m;
            else
                return m;
        }
        return  -1;
    }
};
}


int main()
{
    test_finds();

    return 0;
}









