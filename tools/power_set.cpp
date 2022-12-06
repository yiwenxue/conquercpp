#include <concepts>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

template <typename T, typename... Ts>
using variant_pair = std::variant<std::pair<T, Ts>...>;

int main() {
  //   using data_type = variant_pair<bool, std::string, int, float>;

  //   std::vector<data_type> data{
  //   std::pair{true, std::string("hello")},
  //   std::pair{false, 1},
  //   std::pair{true, 1.0f},
  //   };
}