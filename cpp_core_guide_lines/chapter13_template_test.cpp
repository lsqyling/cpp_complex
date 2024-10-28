#include <vector>
#include <iostream>
#include <algorithm>
#include <concepts>
#include <future>

namespace t40 {
class sum_me
{
public:
    void operator()(int x)
    {
        sum += x;
    }

    [[nodiscard]]
    int get_sum() const { return sum; }

private:
    int sum{0};
};


void test()
{
    std::vector<int> nums{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    sum_me me = std::for_each(nums.begin(), nums.end(), sum_me());
    std::cout << '\n';
    std::cout << "Sum of intVec = " << me.get_sum() << '\n';
    std::cout << '\n';
}
}

namespace t44 {
template<typename T>
void show_me_func(const T &t)
{
    std::cout << t << '\n';
}

template<typename T>
struct show_me_t
{
    explicit show_me_t(const T &t)
    {
        std::cout << t << '\n';
    }
};

void test()
{
    show_me_func(5);
    show_me_func(5.5);
    show_me_t a(5.5);
    show_me_t b(5);
}
}

namespace t46 {

void test()
{
    std::cout << std::boolalpha << '\n';

    std::cout << "std::is_default_constructible_v<int&>: " << std::is_default_constructible_v<int&> << '\n';
    std::cout << "std::is_copy_constructible_v<int&>: " << std::is_copy_constructible_v<int&> << '\n';
    std::cout << "std::is_copy_assignable_v<int&>: " << std::is_copy_assignable_v<int&> << '\n';
    std::cout << "std::is_move_constructible_v<int&>: " << std::is_move_constructible_v<int&> << '\n';
    std::cout << "std::is_move_assignable_v<int&>: " << std::is_move_assignable_v<int&> << '\n';
    std::cout << "std::is_destructible_v<int&>: " << std::is_destructible_v<int&> << '\n';
    std::cout << "std::is_swappable_v<int&>: " << std::is_swappable_v<int&> << '\n';

    std::cout << '\n';
}

}
namespace t48 {
template<typename T,
        typename std::enable_if_t<std::is_integral_v<T>, T> = 0>
T gcd_not_enough_good(T a, T b)
{
    if (b == 0) return a;
    return gcd_not_enough_good(b, a % b);
}

template<std::integral T>
T gcd(T a, T b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}


void test()
{
    std::cout << '\n';

    std::cout << "gcd_not_enough_good(100, 10) = " << gcd_not_enough_good(100, 10) << '\n';
//    std::cout << "gcd_not_enough_good(3.5, 4.0) = " << gcd_not_enough_good(3.5, 4.0) << '\n';

    std::cout << "gcd(100, 10) = " << gcd(100, 10) << '\n';
//    std::cout << "gcd(3.5, 4.0) = " << gcd(3.5, 4.0) << '\n';

    std::cout << '\n';
}
}

namespace t62 {
class array_base
{
public:
    array_base(std::size_t n) : size_(n) {}
    std::size_t get_size() const { return size_; }
private:
    std::size_t size_;
};

template<typename T, size_t N>
class array : private array_base
{
public:
    array() : array_base(N) {}
    size_t get_size() const { return array_base::get_size(); }
private:
    T data_[N];
};


void test()
{
    array<int, 100> arr1;
    array<double, 200> arr2;
    std::cout << "arr1.get_size(): " << arr1.get_size() << '\n';
    std::cout << "arr2.get_size(): " << arr2.get_size() << '\n';
}


class account
{
    friend bool operator<(const account &a, const account &b);

public:
    account() = default;
    account(double b) : balance_(b) {}
private:
    double balance_{0.0};
};

bool operator<(const account &a, const account &b)
{
    return a.balance_ < b.balance_;
}


class shape
{
public:
    template<class T>
           /* virtual */void intersect(T *p) {}
};


void test_virtual_func_template()
{
    shape shape_;
}

}

namespace lambda {

void test_value_ref()
{
    std::cout << std::endl;
    std::cout << std::endl;
    int x = 5, y = 8;
    auto foo = [x, &y]() mutable {
        x += 1;
        y += 2;
        std::cout << "lambda x = " << x << ", y = " << y << std::endl;
        return x * y;
    };

    x = 9;
    y = 20;
    foo();
    std::cout << "call1  x = " << x << ", y = " << y << std::endl;
//    foo();
//    std::cout << "call2  x = " << x << ", y = " << y << std::endl;
}

class work
{
public:
    work() : value_(42) {}
    std::future<int> spawn()
    {
        int x = 3;
        int y = 4;
        return std::async([=, this]() { return value_ * x * y; });
    }

private:
    int value_;
};







}




















int main()
{
    t40::test();
    t44::test();
    t46::test();
    t48::test();
    t62::test();
    t62::test_virtual_func_template();
    lambda::test_value_ref();
    return 0;
}