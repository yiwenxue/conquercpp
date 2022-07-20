#include <iostream>
#include <ranges>
#include <string>

int main(int argc, char *argv[])
{
  using namespace std::ranges;
  std::string s{"This is a string."};

  for (auto v : s | views::split(' '))
  {
    std::string w;
    for (auto &c : v)
    {
      w += c;
    }
    std::cout << w << std::endl;
  }
}