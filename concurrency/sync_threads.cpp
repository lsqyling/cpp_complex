//
// Created by 10580.
//
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <format>
#include <chrono>
#include <queue>

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






int main()
{
    sync_op::test_queue_threadsafe();



    return 0;
}