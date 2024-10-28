#include <string_view>
#include <fstream>
#include <random>
#include "format.h"
#include <memory>
#include <chrono>

using std::cout;
using std::endl;
using std::format;

namespace dependency_injection {
class ilogger
{
public:
    virtual ~ilogger() = default;

    enum class LogLevel
    {
        Error,
        Info,
        Debug
    };

    virtual void set_log_level(LogLevel level) = 0;

    virtual void log(std::string_view msg, LogLevel level) = 0;
};

class logger : public ilogger
{
public:
    explicit logger(std::string_view log_filename);
//    dtor
    ~logger() override = default;

    void set_log_level(LogLevel level) override;

    void log(std::string_view msg, LogLevel level) override;

private:
    [[nodiscard]]
    static std::string get_log_level_string(LogLevel log_level);



    std::ofstream m_output;
    LogLevel m_log_level{LogLevel::Debug};
};

void logger::set_log_level(ilogger::LogLevel level)
{
    m_log_level = level;

}

void logger::log(std::string_view msg, ilogger::LogLevel level)
{
    if (m_log_level < level)
        return;
    m_output << std::format("{}: {}", get_log_level_string(level), msg)
             << "\n";
}

logger::logger(std::string_view log_filename)
{
    m_output.open(log_filename.data(), std::ios_base::app);
    if (!m_output.good())
        throw std::runtime_error("Unable to initialize the logger!");
}

std::string logger::get_log_level_string(ilogger::LogLevel log_level)
{
    switch (log_level)
    {
        case LogLevel::Error:
            return "Error";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Debug:
            return "Debug";
        default:
            throw std::runtime_error("Invalid log level");
    }
}

class foo
{
public:
    explicit foo(ilogger &logger) : m_logger(logger) {}
    void do_something()
    {
        m_logger.log("hello dependency injection!", ilogger::LogLevel::Info);
    }
private:
    ilogger &m_logger;
};

void test()
{
    logger concrete_logger{"log.out"};
    concrete_logger.set_log_level(ilogger::LogLevel::Debug);
    foo f(concrete_logger);
    f.do_something();


    foo f1 = f;
    f1.do_something();
    foo f2(std::move(f));
    f2.do_something();

    foo f3(concrete_logger);
    f3.do_something();
}
}

namespace abstract_factory {
class icar
{
public:
    virtual ~icar() = default;
    [[nodiscard]]
    virtual std::string info() const = 0;
};

class ford : public icar
{
};
class ford_sedan : public ford
{
public:
    [[nodiscard]]
    std::string info() const override
    {
        return "Ford Sedan";
    }
};

class ford_suv : public ford
{
public:
    [[nodiscard]]
    std::string info() const override
    {
        return "Ford SUV";
    }
};

class toyota : public icar
{
};

class toyota_sedan : public toyota
{
public:
    [[nodiscard]]
    std::string info() const override
    {
        return "Toyota Sedan";
    }
};

class toyota_suv : public toyota
{
public:
    [[nodiscard]]
    std::string info() const override
    {
        return "Toyota SUV";
    }
};

class iabstract_car_factory
{
public:
    virtual ~iabstract_car_factory() = default;
    virtual std::unique_ptr<icar> make_suv() = 0;
    virtual std::unique_ptr<icar> make_sedan() = 0;
};

class ford_factory : public iabstract_car_factory
{
public:
    std::unique_ptr<icar> make_suv() override
    {
        return std::make_unique<ford_suv>();
    }

    std::unique_ptr<icar> make_sedan() override
    {
        return std::make_unique<ford_sedan>();
    }

};

class toyota_factory : public iabstract_car_factory
{
public:
    std::unique_ptr<icar> make_suv() override
    {
        return std::make_unique<toyota_suv>();
    }

    std::unique_ptr<icar> make_sedan() override
    {
        return std::make_unique<toyota_sedan>();
    }
};

void create_some_cars(iabstract_car_factory &car_factory)
{
    auto sedan = car_factory.make_sedan();
    auto suv = car_factory.make_suv();
    std::cout << std::format("Sedan: {}\n", sedan->info());
    std::cout << std::format("SUV: {}\n", suv->info());
}

void test()
{
    ford_factory ff;
    toyota_factory tf;
    create_some_cars(ff);
    create_some_cars(tf);
}
}

namespace factory_method {
class icar
{
public:
    virtual ~icar() = default;
    [[nodiscard]]
    virtual std::string info() const = 0;
};

class ford : public icar
{
public:
    [[nodiscard]]
    std::string info() const override
    {
        return "Ford";
    }
};

class toyota : public icar
{
public:
    [[nodiscard]]
    std::string info() const override
    {
        return "Toyota";
    }
};

class car_factory
{
public:
    virtual ~car_factory() = default;
    std::unique_ptr<icar> request_car();
protected:
    virtual std::unique_ptr<icar> create_car() = 0;

private:
    int m_num_of_cars_produced{0};
};

std::unique_ptr<icar> car_factory::request_car()
{
    ++m_num_of_cars_produced;
    return create_car();
}
class ford_factory : public car_factory
{
protected:
    std::unique_ptr<icar> create_car() override
    {
        return std::make_unique<ford>();
    }
};

class toyota_factory : public car_factory
{
protected:
    std::unique_ptr<icar> create_car() override
    {
        return std::make_unique<toyota>();
    }
};

void test()
{
    toyota_factory tf;
    ford_factory ff;
    auto mycar = tf.request_car();
    auto mycar1 = ff.request_car();
    cout << mycar->info() << endl;
    cout << mycar1->info() << endl;

}
}

namespace adapter {
class inew_logger
{
public:
    virtual ~inew_logger() = default;
    virtual void log(std::string_view msg) = 0;
};

class new_logger_adapter : public inew_logger
{
public:
    new_logger_adapter() : m_logger("log.out") {}

    void log(std::string_view msg) override
    {
        m_logger.log(msg, dependency_injection::ilogger::LogLevel::Info);
    }

private:
    dependency_injection::logger m_logger;
};


void test()
{
    new_logger_adapter logger;
    logger.log("Testing the logger.");
}
}

namespace proxy {
 class iplay
 {
 public:
     virtual ~iplay() = default;
     [[nodiscard]]
     virtual std::string get_name() const = 0;
     [[nodiscard]]
     virtual std::string send_instant_msg(std::string_view msg) const = 0;
 };

 class play : public iplay
 {
 public:
     [[nodiscard]]
     std::string get_name() const override;

     [[nodiscard]]
     std::string send_instant_msg(std::string_view msg) const override;
 };

std::string play::get_name() const
{
    return "Play";
}

std::string play::send_instant_msg(std::string_view msg) const
{
    cout << format("play: {}\n", msg);
    return format("play: {}\n", msg);
}

class play_proxy : public iplay
{
public:
    explicit play_proxy(std::unique_ptr<iplay> player);
    [[nodiscard]]
    std::string get_name() const override;

    [[nodiscard]]
    std::string send_instant_msg(std::string_view msg) const override;
private:

    [[nodiscard]]
    static bool has_network_connectivity();

    std::unique_ptr<iplay> m_player;
};

play_proxy::play_proxy(std::unique_ptr<iplay> player) : m_player(std::move(player))
{

}

std::string play_proxy::get_name() const
{
    return m_player->get_name();
}

std::string play_proxy::send_instant_msg(std::string_view msg) const
{
    if (has_network_connectivity())
        return m_player->send_instant_msg(msg);
    else
        return "The player has gone offline";
}

namespace details {
int get_random_num(int left, int right)
{
    std::random_device seeder;
    auto seed = seeder.entropy() != 0 ? seeder() : std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 engine(seed);
    std::uniform_int_distribution<int> u(left, right);
    return u(engine);
}
}


bool play_proxy::has_network_connectivity()
{
    std::random_device seeder;
    auto seed = seeder.entropy() != 0 ? seeder() : std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 engine(seed);
    std::uniform_int_distribution<int> u{1, 99};
    return u(engine) % 2 == 1;
}

bool inform_winner(const iplay &player)
{
    auto r = player.send_instant_msg("you have won! play again: ");
    if (r == "yes")
        cout << player.get_name() << "wants to play again." << endl;
    else
        cout << player.get_name() << " does not want to play again." << endl;
    return details::get_random_num(1, 100) % 2;
}

void test()
{
    play_proxy proxy(std::make_unique<play>());
    inform_winner(proxy);

    cout << "sizeof(proxy) = " << sizeof(proxy) << endl;
}
}


namespace test_noexcept {
struct point
{
    int x, y;
};

void test()
{
    point p{};
    point p1{};

    std::cout << std::format("noexcept(point()) = {}\n", noexcept(point()));
    static_assert(noexcept(point()) == true);
    static_assert(noexcept(p.~point()) == true);
    static_assert(noexcept(point(p1)) == true);
    static_assert(noexcept(point(point{})) == true);
    static_assert(noexcept(p.operator=(p1)) == true);
    static_assert(noexcept(p.operator=(point{})) == true);
}
}







int main()
{
    dependency_injection::test();
    abstract_factory::test();
    factory_method::test();
    adapter::test();
    proxy::test();
    test_noexcept::test();
    return 0;
}