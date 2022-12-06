#include <coroutine>
#include <exception>
#include <future>
#include <iostream>

// lazy iterator
template <typename T>
struct Generator {
    struct promise_type {
        T value_;
        std::exception_ptr exception_;

        auto get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }

        void unhandled_exception() { exception_ = std::current_exception(); }

        void return_void() {}

        auto yield_value(T value) {
            value_ = value;
            return std::suspend_always{};
        }
    };

    struct lazy_iterator {
        Generator& generator_;
        bool done_ = false;

        void operator++() {
            generator_.h_();
            if (generator_.h_.promise().exception_) {
                std::rethrow_exception(generator_.h_.promise().exception_);
            }
            done_ = generator_.h_.done();
        }

        T operator*() const { return generator_.h_.promise().value_; }

        friend bool operator==(const lazy_iterator& lhs, const lazy_iterator& rhs) {
            return lhs.done_ == rhs.done_;
        }

        friend bool operator!=(const lazy_iterator& lhs, const lazy_iterator& rhs) {
            return !(lhs == rhs);
        }
    };

    Generator(std::coroutine_handle<promise_type> h) : h_(h) {}

    ~Generator() { h_.destroy(); }

    lazy_iterator begin() const noexcept {
        return {*const_cast<Generator*>(this)};
    }
    lazy_iterator end() const noexcept {
        return {*const_cast<Generator*>(this), true};
    }

private:
    std::coroutine_handle<promise_type> h_;
};

Generator<int> range(int from, int to) {
    for (int i = from; i < to; ++i) {
        co_yield i;
    }
}

int main() {
    // range based for
    for (auto i : range(0, 10)) {
        std::cout << i << std::endl;
    }

    return 0;
}