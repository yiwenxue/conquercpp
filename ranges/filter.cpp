#include <iostream>
#include <ranges>
#include <vector>
#include <list>
#include <array>

using namespace std::ranges;

int main(int argc, char *argv[])
{
    using namespace std::ranges;
    auto const ints = {0, 1, 2, 3, 4, 5};
    auto even = [](int i)
    { return 0 == i % 2; };

    auto square = [](int i)
    { return i * i; };

    auto e = ints | views::filter(even);

    for (auto i : e)
    {
        std::cout << i << std::endl;
    }

}