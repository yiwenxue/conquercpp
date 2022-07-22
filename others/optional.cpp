#include <functional>
#include <iostream>
#include <optional>
#include <string>

std::optional<std::string> create(bool b) {
  if (b)
    return "Godzilla";
  return {};
}

// std::nullopt can be used to create any (empty) std::optional
auto create2(bool b) {
  return b ? std::optional<std::string>{"Godzilla"} : std::nullopt;
}

// std::reference_wrapper may be used to return a reference
auto create_ref(bool b) {
  static std::string value = "Godzilla";
  return b ? std::optional<std::reference_wrapper<std::string>>{value}
           : std::nullopt;
}

int main() {
  {
    // basic usage
    std::cout << "create(false) returned " << create(false).value_or("empty")
              << '\n';

    // optional-returning factory functions are usable as conditions of while
    // and if
    if (auto str = create2(true)) {
      std::cout << "create2(true) returned " << *str << '\n';
    }

    if (auto str = create_ref(true)) {
      // using get() to access the reference_wrapper's value
      std::cout << "create_ref(true) returned " << str->get() << '\n';
      str->get() = "Mothra";
      std::cout << "modifying it changed it to " << str->get() << '\n';
    }
  }

  {
    // bool cast / has_value
    std::cout << std::boolalpha;

    std::optional<int> opt;
    std::cout << opt.has_value() << '\n';

    opt = 43;
    if (opt)
      std::cout << "value set to " << opt.value() << '\n';
    else
      std::cout << "value not set\n";

    opt.reset();
    if (opt.has_value())
      std::cout << "value still set to " << opt.value() << '\n';
    else
      std::cout << "value no longer set\n";
  }

  return 0;
}
