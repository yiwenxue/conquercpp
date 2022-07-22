#include <concepts>
#include <iostream>
#include <type_traits>

template <typename T>
concept Addable = requires(T t) {
  t + t;
};

// compound
template <typename T>
concept Hashable = requires(T t) {
  { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept Outputable = requires(std::ostream &os, T t) {
  { os << t } -> std::convertible_to<std::ostream &>;
};

template <typename T>
requires Outputable<T> && Hashable<T> && Addable<T>
void f(T a) { std::cout << a << " -> " << std::hash<T>{}(a) << std::endl; }

int main() { f(42); }
