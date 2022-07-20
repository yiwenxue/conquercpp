#include <ranges>
#include <string>
#include <vector>
#include <iostream>

#include <algorithm>

#include <source_location>

int main() {
    using namespace std::ranges;

    std::vector<int> val {1, 2, 3, 4, 6, 5, 7, 8, 9, 10};

    auto operation = views::filter([](const auto &a) {return a % 2 == 0;})
        | views::transform([](const auto &a) {return a * 2;})
        | views::drop(2)
        | views::transform([](const auto &a) {return std::to_string(a);});

    for (auto a : val | operation) {
        std::cout << a << std::endl;
    }

    int a[] = {1, 2, 3, 4, 6, 5, 7, 8, 9, 10};

    std::sort(a, a+9);

    for (auto b : a) 
    {
        std::cout << b << std::endl;
    }

    if (1 and 1) {
        std::cout << "Fuck" << std::endl;
    }

    return 0;
}