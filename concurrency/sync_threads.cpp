//
// Created by 10580.
//
#include <mutex>
#include <thread>
#include <iostream>
#include <format>
#include <chrono>
#include <queue>
#include <future>

using namespace std::chrono_literals;

namespace sync_op {

class condi_sync
{
public:
    void wait_for_flag_low()
    {
        std::unique_lock ul(mut);
        while (!flag)
        {
            ul.unlock();
            ul.lock();
        }
    }

    void wait_for_flag_middle()
    {
        std::unique_lock ul(mut);
        while (!flag)
        {
            ul.unlock();
            std::this_thread::sleep_for(100ms);
            ul.lock();
        }
    }

    void wait_for_flag_high()
    {
        std::unique_lock ul(mut);
        condi.wait(ul, [this] { return flag; });
    }

    void change_flag()
    {
        std::this_thread::sleep_for(100ms);
        {
            std::unique_lock ul{mut};
            flag = true;
        }
        condi.notify_one();
    }

private:
    bool flag{false};
    std::mutex mut;
    std::condition_variable condi;
};

template<typename T>
class queue_threadsafe
{
public:
    void push(T val)
    {
        {
            std::lock_guard lg{m_mut};
            m_data.push(val);
        }
        m_condi.notify_one();
    }

    void pop(T &val)
    {
        std::unique_lock ul{m_mut};
        m_condi.wait(ul, [this] { return !m_data.empty(); });
        val = m_data.front();
        m_data.pop();
    }

    std::shared_ptr<T> pop()
    {
        std::unique_lock ul{m_mut};
        m_condi.wait(ul, [this] { return !m_data.empty(); });
        auto ptr = std::make_shared<T>(m_data.front());
        m_data.pop();
        return ptr;
    }

    bool empty() const
    {
        std::lock_guard lg{m_mut};
        return m_data.empty();
    }


private:
    mutable std::mutex m_mut;
    std::condition_variable m_condi;
    std::queue<T> m_data;
};

void producer(queue_threadsafe<int> &q)
{
    for (int i = 0; i < 5; ++i)
    {
        q.push(i);
        std::cout << std::format("push: {}\n", i);
    }
}

void consumer(queue_threadsafe<int> &q)
{
    int v;
    for (int i = 0; i < 5; ++i)
    {
        q.pop(v);
        std::cout << std::format("pop: {}\n", v);
    }
}

void test_queue_threadsafe()
{
    queue_threadsafe<int> q;
    std::thread t1{consumer, std::ref(q)};
    std::thread t0{producer, std::ref(q)};

    t1.join();
    t0.join();
}
}

namespace future_op {

int task(int n)
{
    std::cout << "异步任务 ID: " << std::this_thread::get_id() << std::endl;
    return n * n;
}

void test_future()
{
    auto fut = std::async(task, 100);
    std::cout << "current ID: " << std::this_thread::get_id() << std::endl;
    std::cout << std::format("future.valid = {}\n", fut.valid());
    std::cout << std::format("future.get() = {}\n", fut.get());
    std::cout << std::format("future.valid = {}\n", fut.valid());
}

struct X
{
    int operator()(int n) const
    {
        return n * n;
    }
};

struct Y
{
    [[nodiscard]]
    int fy(int n) const
    {
        return n * n;
    }
};

void f(int &p)
{
    std::cout << std::format("&p = {:p}\n", reinterpret_cast<void *>(&p));
}

void test_async()
{
    Y y;
    int n = 10;
    auto fut0 = std::async(X{}, 10);
    auto fut1 = std::async(&Y::fy, y, 10);
    auto fut2 = std::async([] {});
    auto fut3 = std::async(f, std::ref(n));
    std::cout << std::format("&n = {:p}\n", reinterpret_cast<void *>(&n));
}
}





int main()
{
    sync_op::test_queue_threadsafe();
    future_op::test_future();
    future_op::test_async();


    return 0;
}