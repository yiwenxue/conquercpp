#include <array>
#include <cassert>
#include <concepts>
#include <iostream>
#include <type_traits>
#include <vector>

// I need a point template for N dimensions, and a point_t type alias for that
// point template, and can be view as array of N dimensions.
template <typename T, std::size_t N>
requires std::is_arithmetic_v<T>
class Point {
public:
  Point() = default;
  Point(const Point &) = default;
  Point(Point &&) = default;
  Point &operator=(const Point &) = default;
  Point &operator=(Point &&) = default;
  ~Point() = default;
  Point(std::array<T, N> &&arr) : data(std::move(arr)) {}
  Point(const std::array<T, N> &arr) : data(arr) {}
  Point &operator=(const std::array<T, N> &arr) {
    static_assert(N == arr.size());
    data = arr;
    return *this;
  }
  Point &operator=(std::array<T, N> &&arr) {
    static_assert(std::is_move_assignable_v<std::array<T, N>>);
    data = std::move(arr);
    return *this;
  }
  const T &operator[](std::size_t i) const {
    assert(i < N);
    return data[i];
  }
  T &operator[](std::size_t i) {
    assert(i < N);
    return data[i];
  }

private:
  std::array<T, N> data;
};

int main(int argc, char **argv) { return 0; }