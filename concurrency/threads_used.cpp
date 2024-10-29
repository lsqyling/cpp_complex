//
// Created by 10580.
//
#include <iostream>
#include <thread>
#include <numeric>
#include <vector>
#include <random>
#include <format>
#include <cassert>
#include <execution>

void hello()
{
    std::cout << "hello thread!" << std::endl;
}

void test_hello()
{
    std::thread t{hello};
    t.join();
}

template<typename ForwardIt>
auto parallel_sum(ForwardIt begin, ForwardIt end)
{
    using value_type = typename std::iterator_traits<ForwardIt>::value_type;
    auto num_threads = std::thread::hardware_concurrency();
    auto distance = std::distance(begin, end);

    if (distance > 1024000)
    {
        auto chunk_size = distance / num_threads;
        auto remainder = distance % num_threads;

        std::vector<value_type> result(num_threads);
        std::vector<std::thread> threads;

        auto start = begin;
        for (int i = 0; i < num_threads; ++i)
        {
            auto next = std::next(start, chunk_size + (i < remainder ? 1 : 0));
            threads.emplace_back([start, next, i, &result]{
                result[i] = std::accumulate(start, next, value_type{});
            });
            start = next;
        }

        for (auto &t: threads)
            t.join();

        auto sum = std::accumulate(result.begin(), result.end(), value_type{});
        return sum;
    }
    value_type total_sum = std::accumulate(begin, end, value_type{});
    return total_sum;
}


void generator_random(int n, std::vector<long> &data)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist(0, 1000);
    for (int i = 0; i < n; ++i)
    {
        data.push_back(dist(gen));
    }
}

void test_parallel_seq_sum()
{
    std::vector<long> data;
    generator_random(10240001, data);

    auto st0 = std::chrono::high_resolution_clock::now();
    auto result0 = parallel_sum(data.begin(), data.end());
    auto et0 = std::chrono::high_resolution_clock::now();
    auto d0 = std::chrono::duration_cast<std::chrono::milliseconds>(et0 - st0);

    auto st1 = std::chrono::high_resolution_clock::now();
    auto result1 = std::accumulate(data.begin(), data.end(), 0L);
    auto et1 = std::chrono::high_resolution_clock::now();
    auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>(et1 - st1);

    auto st2 = std::chrono::high_resolution_clock::now();
    auto result2 = std::reduce(std::execution::par, data.begin(), data.end());
    auto et2 = std::chrono::high_resolution_clock::now();
    auto d2 = std::chrono::duration_cast<std::chrono::milliseconds>(et2 - st2);



    assert(result0 == result1 && "the answer is wrong!");
    assert(result1 == result2 && "the answer is wrong!");
    std::cout << std::format("parallel_sum: {} ms, sequential_sum: {} ms\n"
                             "standard_parallel_sum: {} ms\nresult = {}\n",
                             d0.count(), d1.count(), d2.count(), result0);
}




int main()
{
    test_hello();
    test_parallel_seq_sum();







    return 0;
}

