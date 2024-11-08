#include <iostream>
#include <random>
#include <vector>
#include <format>
#include <algorithm>

int gen_random(int max)
{
    static std::random_device rd;
    static std::mt19937 engine{rd()};
    static std::uniform_int_distribution<int> dist(0, max);

    return dist(engine);
}


void experiment(std::vector<double> &all, int loops)
{
    int n = static_cast<int>(all.size());
    std::vector<bool> has_money(n, false);
    for (int k = 0; k < loops; ++k)
    {
        std::fill(has_money.begin(), has_money.end(), false);
        for (int i = 0; i < n; ++i)
        {
            if (all[i] > 0)
                has_money[i] = true;
        }

        for (int i = 0; i < n; ++i)
        {
            if (has_money[i])
            {
                int j = i;
                do
                {
                    j = gen_random(n-1);
                } while (j == i);
                --all[i];
                ++all[j];
            }
        }
    }
}


double calc_gini(const std::vector<double> &all)
{
    double sum = 0;
    double total_diff = 0;
    int n = static_cast<int>(all.size());
    for (int i = 0; i < n; ++i)
    {
        sum += all[i];
        for (int j = 0; j < n; ++j)
        {
            total_diff += std::abs(all[i] - all[j]);
        }
    }

    double gini = total_diff / (1.0 * 2 * n * sum);
    return gini;
}

void entry()
{
    std::cout << "一个社会的基尼系数是在0-1之间的小数；"  << std::endl;
    std::cout << "基尼系数为0代表所有人的财富绝对均匀，"  << std::endl;
    std::cout << "基尼系数为1代表1个人掌握了社会所有的财富。"  << std::endl;
    std::cout << "基尼系数越小，代表社会财富分布越均匀；反之，代表社会财富分布越不均匀；"  << std::endl;
    std::cout << "在2022年，世界各国的平均基尼为0.44。目前普遍认为，"  << std::endl;
    std::cout << "当基尼系数达到0.5时，就意为着社会财富分布非常不均匀。"  << std::endl;
    std::cout << "社会可能陷入危机，比如大量的犯罪或社会动荡。"  << std::endl;
    std::cout << "测试开始......"  << std::endl;

    int n = 100;
    std::vector<double> money(n, 100);
    long t = 1;

    double gini = 0;
    do
    {
        experiment(money, t);
        gini = calc_gini(money);
        ++t;
    } while (gini <= 0.5);

    std::cout << "此时社会已经达到很危险的程度了！！！" << std::endl;
    std::cout << std::format("此时经历的轮数为：{}， 人数：{}\n", t, n);
    std::cout << std::format("此时社会实验的基尼系数为：{:.6f}.\n", gini);

    std::sort(money.begin(), money.end());
    for (int i = 0; i < money.size(); ++i)
    {
        std::cout << money[i] << " ";
        if (i && i % 10 == 0)
            std::cout << std::endl;
    }
}




int main()
{

    entry();



    return 0;
}

