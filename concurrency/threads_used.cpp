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

namespace thr {
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
            threads.emplace_back([start, next, i, &result] {
                result[i] = std::accumulate(start, next, value_type{});
            });
            start = next;
        }

        for (auto &t: threads)
        {
            t.join();
        }

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
    generator_random(1024001, data);

    auto st0 = std::chrono::high_resolution_clock::now();
    auto result0 = parallel_sum(data.begin(), data.end());
    auto et0 = std::chrono::high_resolution_clock::now();
    auto d0 = std::chrono::duration_cast<std::chrono::microseconds>(et0 - st0);

    auto st1 = std::chrono::high_resolution_clock::now();
    auto result1 = std::accumulate(data.begin(), data.end(), 0L);
    auto et1 = std::chrono::high_resolution_clock::now();
    auto d1 = std::chrono::duration_cast<std::chrono::microseconds>(et1 - st1);

    auto st2 = std::chrono::high_resolution_clock::now();
    auto result2 = std::reduce(std::execution::par, data.begin(), data.end());
    auto et2 = std::chrono::high_resolution_clock::now();
    auto d2 = std::chrono::duration_cast<std::chrono::microseconds>(et2 - st2);


    assert(result0 == result1 && "the answer is wrong!");
    assert(result1 == result2 && "the answer is wrong!");
    std::cout << std::format("parallel_sum: {} us, sequential_sum: {} us\n"
                             "standard_parallel_sum: {} us\nresult = {}\n",
                             d0.count(), d1.count(), d2.count(), result0);
}

struct task
{
    void operator()() const
    {
        std::cout << "operator()() const" << std::endl;
    }
};
//std::thread t(task());


void test_task()
{

    std::thread t(task{});
    t.join();//ok

    std::thread t1{task{}};
    t1.join();//ok
}
}

namespace thr_src {

class thread_guard
{
public:
    explicit thread_guard(std::thread &t) : m_thr(t)
    {}

    ~thread_guard()
    {
        if (m_thr.joinable())
            m_thr.join();
    }

    thread_guard(thread_guard &&) = delete;

    thread_guard &operator=(const thread_guard &) = delete;

private:
    std::thread &m_thr;
};

void test_thread_guard()
{
    std::thread t{[] { std::cout << "thr_guard dtor." << std::endl; }};
    thread_guard g(t);
}

struct X
{
    X(X &&) noexcept{}

    template<typename Fn, typename ...Args>
    requires (!std::is_same_v<std::remove_cvref_t<Fn>, X>)
    explicit X(Fn &&fn, Args &&...args) {}

    X(const X &) = delete;
};

void test_X()
{
    X x([] {});
//    X x1(x);// compile error.
}

class join_thread
{
public:
    join_thread() = default;
    join_thread(join_thread &&other) noexcept : m_thr(std::exchange(other.m_thr, {})) {}
    explicit join_thread(std::thread thr) noexcept : m_thr(std::exchange(thr, {})) {}
    join_thread &operator=(std::thread &&other) noexcept
    {
        if (m_thr.joinable())
            m_thr.join();
        m_thr = std::move(other);
        return *this;
    }

    ~join_thread()
    {
        if (m_thr.joinable())
            m_thr.join();
    }

    [[nodiscard]]
    std::thread::id get_id() const
    {
        return m_thr.get_id();
    }

    void join()
    {
        m_thr.join();
    }

    void detach()
    {
        m_thr.detach();
    }

    std::thread &data()
    {
        return m_thr;
    }

    [[nodiscard]]
    const std::thread &data() const
    {
        return m_thr;
    }

private:
    std::thread m_thr;
};
class T {};
void test_join_thread()
{
    join_thread jthr;
    std::cout << std::format("join_thread dtor is noexcept,true or false ? {}\n", noexcept(test_X()));
    std::cout << std::format("noexcept(T()): {}\n", noexcept(T{}));
    std::cout << std::format("noexcept(T(T())): {}\n", noexcept(T(T{})));
    T t, t1;
    std::cout << std::format("noexcept(T(t)): {}\n", noexcept(T{t}));
    std::cout << std::format("noexcept(t.operator=(T{{}})): {}\n", noexcept(t.operator=(T{})));
    std::cout << std::format("noexcept(t.operator=(t1)): {}\n", noexcept(t.operator=(t1)));
    std::cout << std::format("noexcept(t.operator=(t.~T())): {}\n", noexcept(t.~T()));
}
}


int main()
{
    using namespace thr;
    test_hello();
    test_parallel_seq_sum();
    test_task();
    thr_src::test_thread_guard();
    thr_src::test_X();
    thr_src::test_join_thread();


    return 0;
}

