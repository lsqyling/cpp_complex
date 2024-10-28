#include <iostream>
#ifdef HAS_CO_CONTEXT

#include <co_context/all.hpp>
#include <random>

using std::cout;
using std::endl;

namespace perfect_g {
using namespace co_context;
generator<int> fibonacci(int a0, int a1)
{
    co_yield a0;
    co_yield a1;
    for (int a2; ; )
    {
        a2 = a0 + a1;
        a0 = a1;
        a1 = a2;
        co_yield a2;
    }
}

void test_fibonacci()
{
    for (int len = 10; auto i : fibonacci(1, 1))
    {
        cout << i << endl;
        if (--len == 0)
            break;
    }
}

co_context::generator<int> gen_iota(int x)
{
    while (true)
    {
        co_yield x++;
    }
}

void test_gen_iota()
{
    using namespace std::views;

    for (int x: gen_iota(1) | drop(5) | take(3))
    {
        std::cout << x << " ";
    }
    // 输出 6 7 8
}
}

namespace my_generator {
template<typename T>
class generator
{
public:
    class promise_type;

    generator(std::coroutine_handle<promise_type> handle) : m_handler(handle) {}
    generator(const generator &) = delete;
    generator &operator=(const generator &) = delete;
    generator(generator &&other) noexcept;
    ~generator();

    void next() { m_handler.resume(); }
    const T &result() const { return m_handler.promise().result(); }
    T &result() { return m_handler.promise().result(); }

private:
    std::coroutine_handle<promise_type> m_handler;
};

template<typename T>
generator<T>::generator(generator &&other) noexcept
{
    if (this != &other)
    {
        m_handler = other.m_handler;
        other.m_handler = nullptr;
    }
}

template<typename T>
generator<T>::~generator()
{
    if (m_handler)
        m_handler.destroy();
}

template<typename T>
class generator<T>::promise_type
{
public:
    promise_type() = default;
    generator<T> get_return_object() noexcept
    {
        return generator<T>(std::coroutine_handle<promise_type>::from_promise(*this));
    }
    void return_void() const noexcept {}
    std::suspend_always initial_suspend() const noexcept { return {}; }
    std::suspend_always final_suspend() const noexcept { return {}; }

    const T &result() const { return m_value.value(); }
    T &result() { return m_value.value(); }
    std::suspend_always yield_value(const T &value) noexcept
    {
        m_value = value;
        return {};
    }

    std::suspend_always yield_value(T &&value) noexcept
    {
        m_value.emplace(std::move(value));
        return {};
    }

    void unhandled_exception() { m_exception = std::current_exception(); }


private:
    std::optional<T> m_value;
    std::exception_ptr m_exception;
};

generator<std::string> lottery(size_t size, unsigned mod)
{
    std::mt19937 engine(time(nullptr));
    std::string winning_numbers;

    co_yield winning_numbers += std::to_string(engine() % mod);
    for (size_t i = 0; i < size; ++i)
    {
        winning_numbers += " ";
        co_yield winning_numbers += std::to_string(engine() % mod);
    }
}

void test_lottery()
{
    auto fi = lottery(10, 256);
    for (int i = 0; i < 10; ++i)
    {
        fi.next();
        cout << fi.result() << endl;
    }
}
}

namespace redis_ping {
using namespace co_context;

constexpr uint16_t PORT = 6379;
constexpr size_t BUF_SIZE = 128;


task<> reply(co_context::socket sock)
{
    char recv_buf[BUF_SIZE];
    int n = co_await sock.recv(recv_buf);
    while(n > 0)
    {
        n = co_await (sock.send({"+OK\r\n", 5}) && sock.recv(recv_buf));
    }
}

task<> server()
{
    acceptor ac{co_context::inet_address{PORT, false}};
    for (int sock_fd; (sock_fd = co_await ac.accept()) >= 0; )
    {
        co_context::co_spawn(reply(co_context::socket(sock_fd)));
    }
}


void test()
{
    io_context ctx;
    ctx.co_spawn(server());
    ctx.start();
    ctx.join();
}

task<> one_clock()
{
    using namespace std::chrono_literals;
    for (int cnt = 0; ; )
    {
        printf("Time = %d\n", cnt++);
        fflush(nullptr);
        co_await timeout(1s);
    }
}

void test_one_clock()
{
    io_context ctx;
    ctx.co_spawn(one_clock());
    ctx.start();
    ctx.join();
}
}

namespace coroutine_scheduler {
using namespace co_context;

task<> a()
{
    printf("a\n");
    co_return ;
}

task<> b()
{
    co_spawn(a());
    printf("b\n");
    co_return ;
}

task<> c(io_context &io)
{
    io.co_spawn(a());
    printf("c\n");
    co_return ;
}

void test()
{
    io_context io;
    io.co_spawn(a());
    io.co_spawn(b());
    io.co_spawn(c(io));
    io.start();
    io.join();
}
}

namespace timer_test {
using namespace co_context;
using namespace std::chrono_literals;
task<> cycle(int sec, const char *message)
{
    while (true)
    {
        co_await timeout(std::chrono::seconds{sec});
        printf("%s\n", message);
        fflush(nullptr);
    }
}

void test_cycle()
{
    io_context ctx;
    ctx.co_spawn(cycle(1, "1 secs"));
    ctx.co_spawn(cycle(2, "\t3 secs"));
    ctx.start();
    ctx.join();
}
}


int main()
{
    perfect_g::test_gen_iota();
    perfect_g::test_fibonacci();
    my_generator::test_lottery();
//    redis_ping::test();
//    redis_ping::test_one_clock();

    coroutine_scheduler::test();
    timer_test::test_cycle();

    return 0;
}

#else

int main()
{
    std::cout << "co_context missing!" << std::endl;

    return 0;
}

#endif