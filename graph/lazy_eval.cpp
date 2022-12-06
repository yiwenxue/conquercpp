#include <any>
#include <concepts>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>

/**
 implement a lazy evaluation framework
 the lazyness means:
 1. the evaluation is delayed until the result is needed;
 2. the evaluation is performed only once
 3. the result is cached
 4. the result is immutable
 5. the need of evaluation is can propagated to the dependent nodes
*/

template <typename T> struct Lazy {
  using value_type = T;
  using function_type = std::function<T()>;
  using optional_type = std::optional<T>;
};

int main() {}

// Monad with lazy evaluation
