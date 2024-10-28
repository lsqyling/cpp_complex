#include <iostream>
#ifdef HAS_CO_CONTEXT
#include <co_context/all.hpp>


co_context::generator<int> gen_iota(int x)
{
    while (true)
        co_yield x++;
}

void test_co_context_generator()
{
    using namespace std::views;
    for (int x: gen_iota(1) | drop(5) | take(3))
    {
        std::cout << x << " ";
    }
    // 输出 6 7 8
}

namespace coroutine {
template<typename T>
class generator : public std::ranges::view_interface<generator<T>>
{

public:
    struct promise_type;

    explicit generator(std::coroutine_handle<promise_type> handle) : handle_(handle)
    {}

    explicit generator(T val) : val_(val)
    {}

    struct promise_type
    {
        [[nodiscard]]
        constexpr std::suspend_never initial_suspend() const { return {}; }
        constexpr std::suspend_never final_suspend() noexcept { return {}; }
        constexpr generator<T> get_return_object()
        {
            return generator<T>(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        std::suspend_always yield_value(const T &value)
        {

            return {};
        }
        void unhandled_exception() {}
    };

    T next() requires requires(T t) { ++t; t++; }
    {
        return val_++;
    }

    T val_;
    std::coroutine_handle<promise_type> handle_{};
};



generator<int> gen_fib(int a0, int a1)
{
    co_yield a0;
    co_yield a1;

    while (true)
    {
        int a2 = a0 + a1;
        co_yield a2;
        a0 = a1;
        a1 = a2;
    }
}

void test_gen_fib()
{
    auto g = gen_fib(1, 1);
    for (int i = 0; i < 10; ++i)
    {
        std::cout << g.next() << std::endl;
    }

}
}



int main()
{
    test_co_context_generator();
    coroutine::test_gen_fib();
    return 0;
}

#else
int main()
{
    std::cout << "co_context missing!" << std::endl;

    return 0;
}

#endif