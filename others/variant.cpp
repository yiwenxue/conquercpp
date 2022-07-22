#include <cassert>
#include <iostream>
#include <string>
#include <variant>

struct S {
  S(int i) : i(i) {}
  int i;
};

using Var = std::variant<int, int, int, std::string>;

template <unsigned I> void print(Var const &var) {
  std::cout << "get<" << var.index() << "> = " << std::get<I>(var)
            << "\t"
               "# = "
            << std::hash<Var>{}(var) << '\n';
}

int main() {
  {
    // basic usage
    std::variant<int, float> v, w;
    v = 42; // v contains int
    int i = std::get<int>(v);
    assert(42 == i); // succeeds
    w = std::get<int>(v);
    w = std::get<0>(v); // same effect as the previous line
    w = v;              // same effect as the previous line

    //  std::get<double>(v); // error: no double in [int, float]
    //  std::get<3>(v);      // error: valid index values are 0 and 1

    try {
      std::get<float>(w); // w contains int, not float: will throw
    } catch (const std::bad_variant_access &ex) {
      std::cout << ex.what() << '\n';
    }

    using namespace std::literals;

    std::variant<std::string> x("abc");
    // converting constructors work when unambiguous
    x = "def"; // converting assignment also works when unambiguous

    std::variant<std::string, void const *> y("abc");
    // casts to void const * when passed a char const *
    assert(std::holds_alternative<void const *>(y)); // succeeds
    y = "xyz"s;
    assert(std::holds_alternative<std::string>(y)); // succeeds
  }

  { // mono state
    std::variant<std::monostate, S> var;
    assert(var.index() == 0);

    try {
      std::get<S>(var); // throws! We need to assign a value
    } catch (const std::bad_variant_access &e) {
      std::cout << e.what() << '\n';
    }

    var = 12;

    std::cout << std::get<S>(var).i << '\n';
  }

  {
    // holds_alternative
    std::variant<int, std::string> v = "abc";
    std::cout << std::boolalpha << "variant holds int? "
              << std::holds_alternative<int>(v) << '\n'
              << "variant holds string? "
              << std::holds_alternative<std::string>(v) << '\n';
  }

  {
    // get_if
    auto check_value = [](const std::variant<int, float> &v) {
      if (const int *pval = std::get_if<int>(&v))
        std::cout << "variant value: " << *pval << '\n';
      else
        std::cout << "failed to get value!" << '\n';
    };

    std::variant<int, float> v{12}, w{3.f};
    check_value(v);
    check_value(w);
  }

  {
    // hash support
    Var var;
    std::get<0>(var) = 2020;
    print<0>(var);
    var.emplace<1>(2023);
    print<1>(var);
    var.emplace<2>(2026);
    print<2>(var);
    var = "C++";
    print<3>(var);
  }
}
