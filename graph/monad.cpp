#include <any>
#include <concepts>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>

using std::function;

// concepts
// 1. left identity: e is left identity, if e * a = a for all a
// 2. right identity: e is right identity, if a * e = a for all a
// 3. two-sided identity (associativity): e is two-sided identity, if e is both
// left and right identity

// monad concepts:
// A monad can be created by defining a type constructor M and two operations,
// unit and bind, satisfying the monad laws.
// 1. unit wraps a value in a monad: unit(a) = M(a)
// 2. bind combines a monad with a function on type A to produce a monad on type
// B, it unboxes the monad, applies the function, and then reboxes the result:
// bind(M(a), f) = M(f(a))
// 3. left identity: unit(a) * f = f(a)
// 4. right identity: m * unit = m
// 5. associativity: (m * f) * g = m * (f * g)

// Monad
template <template <typename...> class F> struct Monad {
  // uint operation: unit :: a -> m a
  template <typename A> static F<A> unit(A a);

  // bind operation: bind :: m a -> (a -> m b) -> m b
  template <typename A, typename B>
  static F<B> bind(F<A> m, function<F<B>(A)> f);
};

// Functor
template <template <typename...> class F> struct Functor {
  // fmap operation: fmap :: (a -> b) -> (f a -> f b)
  template <typename A, typename B>
  static function<F<B>(F<A>)> fmap(function<B(A)> f);

  // pure operation: pure :: a -> f a
  template <typename A> static F<A> pure(A a);

  // join operation: join :: m (m a) -> m a
  template <typename A> static F<A> join(F<F<A>> m);
};

// operators uint
template <template <typename...> class F, typename A> static F<A> unit(A a) {
  return Monad<F>::unit(a);
}

// operators bind
template <template <typename...> class F, typename A, typename B>
static F<B> bind(F<A> m, function<F<B>(A)> f) {
  return Monad<F>::bind(m, f);
}

// operators fmap
template <template <typename...> class F, typename A, typename B>
static function<F<B>(F<A>)> fmap(function<B(A)> f) {
  return Functor<F>::fmap(f);
}

// operators fmap
template <template <typename...> class F, typename A, typename B>
static F<B> fmap(F<A> m, function<B(A)> f) {
  return Functor<F>::fmap(f)(m);
}

// operators pure
template <template <typename...> class F, typename A> static F<A> pure(A a) {
  return Functor<F>::pure(a);
}

// operators join
template <template <typename...> class F, typename A>
static F<A> join(F<F<A>> m) {
  return Functor<F>::join(m);
}

// operators compose
template <typename A, typename B, typename C>
static function<C(A)> compose(function<B(A)> f, function<C(B)> g) {
  return [f, g](A a) { return g(f(a)); };
}

template <typename T>
concept streamable = requires(T a) {
  { std::cout << a } -> std::same_as<std::ostream &>;
  { std::cin >> a } -> std::same_as<std::istream &>;
};

// Identity
template <typename A> struct Identity {
  A value;
  Identity(A value) : value(value) {}
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const Identity<T> &id) {
  os << id.value;
  return os;
}
template <typename T>
std::istream &operator>>(std::istream &is, Identity<T> &id) {
  is >> id.value;
  return is;
}

// Monad instance for Identity
template <> struct Monad<Identity> {
  template <typename A> static Identity<A> unit(A a) { return Identity<A>(a); }

  template <typename A, typename B>
  static Identity<B> bind(Identity<A> m, function<Identity<B>(A)> f) {
    return f(m.value);
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::optional<T> &opt) {
  if (opt.has_value()) {
    os << "Just \"" << opt.value() << "\"";
  } else {
    os << "Nothing";
  }
  return os;
}
template <typename T>
std::istream &operator>>(std::istream &is, std::optional<T> &opt) {
  T value;
  is >> value;
  opt = value;
  return is;
}

// Monad instance for std::optional
template <> struct Monad<std::optional> {
  template <typename A> static std::optional<A> unit(A a) { return a; }

  template <typename A, typename B>
  static std::optional<B> bind(std::optional<A> m,
                               function<std::optional<B>(A)> f) {
    if (m) {
      return f(*m);
    } else {
      return std::nullopt;
    }
  }
};

// Functor instance for Identity
template <> struct Functor<Identity> {
  template <typename A, typename B>
  static function<Identity<B>(Identity<A>)> fmap(function<B(A)> f) {
    return [f](Identity<A> a) { return Identity<B>(f(a.value)); };
  }

  template <typename A> static Identity<A> pure(A a) { return Identity<A>(a); }

  template <typename A> static Identity<A> join(Identity<Identity<A>> m) {
    return m.value;
  }
};

// Functor instance for std::optional
template <> struct Functor<std::optional> {
  template <typename A, typename B>
  static function<std::optional<B>(std::optional<A>)> fmap(function<B(A)> f) {
    return [f](std::optional<A> m) -> std::optional<B> {
      if (m) {
        return f(*m);
      } else {
        return std::nullopt;
      }
    };
  }

  template <typename A> static std::optional<A> pure(A a) { return a; }

  template <typename A>
  static std::optional<A> join(std::optional<std::optional<A>> mma) {
    if (mma) {
      return *mma;
    } else {
      return std::nullopt;
    }
  }
};

// utils to consume the side effect, print
template <template <typename...> class F> void print(Monad<F> v) {
  std::cout << v << std::endl;
};

template <typename T> using Maybe = std::optional<T>;

int main() {
  // test optional
  auto f = [](int x) {
    std::cout << "f(" << x << ")" << std::endl;
    return x + 1;
  };

  std::cout << "test optional" << std::endl;

  auto i = unit<std::optional, int>(1);

  auto j = bind<std::optional, int, int>(i, f);
  std::cout << "test optional" << std::endl;
  auto k = bind<std::optional, int, int>(j, f);
  std::cout << "test optional" << std::endl;
  auto fm = fmap<std::optional, int, int>(f);

  auto l = fmap<std::optional, int, int>(k, f);
  std::cout << "test optional" << std::endl;
  auto m = fm(l);
  std::cout << "test optional" << std::endl;

  std::cout << "i: " << i << std::endl;
  return 0;
}

//