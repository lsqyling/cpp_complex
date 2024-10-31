//
// Created by 10580.
//

#include <thread>
#include <iostream>
#include <format>
#include <utility>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <map>

using namespace std::chrono_literals;

namespace data_race {

void f()
{
    std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
}
std::mutex mut;
void rf()
{
    std::lock_guard<std::mutex> lg{mut};
    std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
}

void test_simple_data_race()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i)
        threads.emplace_back(f);
    for (auto &thr : threads)
        thr.join();
}

std::mutex m0;
void thr_func(int id)
{
    if (m0.try_lock())
    {
        std::cout << std::format("thread: {} get the lock.\n", id);
        std::this_thread::sleep_for(1s);
        m0.unlock();
        std::cout << std::format("thread: {} release the lock.\n", id);
    }
    else
    {
        std::cout << std::format("thread: {} get the lock failed!\n", id);
    }
}

void test_thr_func()
{
    std::thread t1{thr_func, 1};
    std::thread t2{thr_func, 2};
    std::thread t3{thr_func, 3};
    std::thread t4{thr_func, 4};

    t1.join();t2.join();t3.join();t4.join();
}

class X
{
    friend void swap(X &lhs, X &rhs);
public:
    explicit X(std::string str) : m_str(std::move(str)) {}

private:
    std::string m_str;
    std::mutex m_mut;
};

void swap(X &lhs, X &rhs)
{
    if (std::addressof(lhs) == std::addressof(rhs))
        return ;
    std::scoped_lock sl(lhs.m_mut, rhs.m_mut);
    swap(lhs.m_str, rhs.m_str);
}
}

namespace shared_init {

struct some {};
std::shared_ptr<some> ptr;
std::once_flag flag;

void init_resource()
{
    ptr = std::make_shared<some>();
}

void foo()
{
    std::call_once(flag, init_resource);
}


struct my_class {};

inline my_class &get_my_class_instance()
{
    static my_class instance;
    return instance;
}

class settings
{
public:
    void set(const std::string &key, const std::string &value)
    {
        std::lock_guard lg(m_mut);
        m_data[key] = value;
    }

    [[nodiscard]]
    std::string get(const std::string &key) const
    {
        std::shared_lock sl(m_mut);
        auto it = m_data.find(key);
        if (it != m_data.end())
            return it->second;
        return {};
    }

private:
    std::map<std::string, std::string> m_data;
    mutable std::shared_mutex m_mut;
};






}



int main()
{
//    data_race::test_simple_data_race();
    data_race::test_thr_func();

    return 0;
}