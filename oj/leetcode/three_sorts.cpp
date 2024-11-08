#include "utilities.hpp"
#include <thread>
void select_sort(std::vector<int> &arr, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        int min_index = i;
        for (int j = i + 1; j < n; ++j)
        {
            if (arr[j] < arr[min_index])
                min_index = j;
        }
        std::swap(arr[i], arr[min_index]);
    }
}

void bubble_sort(std::vector<int> &arr, int n)
{
    for (int i = n - 1; i > 0; --i)
    {
        for (int j = 0; j < i; ++j)
        {
            if (arr[j] > arr[j+1])
                std::swap(arr[j], arr[j+1]);
        }
    }
}

void insert_sort(std::vector<int> &arr, int n)
{
    for (int i = 1; i < n; ++i)
    {
        int j, k = arr[i];
        for (j = i - 1; j >= 0 && k < arr[j]; --j)
            arr[j+1] = arr[j];

        arr[j+1] = k;
    }
}

void thr0(int n, int max)
{
    auto arr0 = gen_rd_array(n, 0, max);
    select_sort(arr0, n);
    std::cout << std::format("select_sort {}!\n", is_sorted(arr0) ? "passed" : "failed");
}
void thr1(int n, int max)
{
    auto arr1 = gen_rd_array(n, 0, max);
    bubble_sort(arr1, n);
    std::cout << std::format("bubble_sort {}!\n", is_sorted(arr1) ? "passed" : "failed");
}
void thr2(int n, int max)
{
    auto arr2 = gen_rd_array(n, 0, max);
    insert_sort(arr2, n);
    std::cout << std::format("insert_sort {}!\n", is_sorted(arr2) ? "passed" : "failed");
}




int main()
{
    int n = 1'000'0;
    int max = 1'000'0;

    std::thread t0(thr0, n, max);
    std::thread t1(thr1, n, max);
    std::thread t2(thr2, n, max);

    t0.join();
    t1.join();
    t2.join();

    return 0;
}