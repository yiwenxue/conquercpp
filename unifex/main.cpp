#include <unifex/any_scheduler.hpp>
#include <unifex/scheduler_concepts.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/task.hpp>
#include <unifex/then.hpp>
#include <unifex/timed_single_thread_context.hpp>
#include <unifex/when_all.hpp>
#include <unifex/let_value.hpp>

#include <unifex/on.hpp>
#include <unifex/stop_when.hpp>

// extra
#include <unifex/async_scope.hpp>
#include <unifex/for_each.hpp>
#include <unifex/receiver_concepts.hpp>
// extra done

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <optional>
#include <chrono>

using namespace unifex;
using namespace std::chrono;
using namespace std::chrono_literals;

// a demo to read a file using coroutines
void demo_read_file()
{
    timed_single_thread_context context;

    auto scheduler = context.get_scheduler();

    auto start = steady_clock::now();

    auto read_file = [&]() -> task<std::string>
    {
        std::ifstream file("main.cpp");
        std::string str;

        file.seekg(0, std::ios::end);
        str.reserve(file.tellg());
        file.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());

        co_return str;
    };

    auto print_file = [](std::string str) -> task<void>
    {
        std::cout << str << std::endl;
        co_return;
    };

    auto print_time = [&]() -> task<void>
    {
        auto end = steady_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        std::cout << "Time taken by function: "
                  << duration.count() << " milliseconds" << std::endl;
        co_return;
    };
}

void demo_read_files()
{
    timed_single_thread_context context;

    auto scheduler = context.get_scheduler();

    auto start = steady_clock::now();
}

void demo_when_all()
{
    timed_single_thread_context context;
    auto scheduler = context.get_scheduler();

    auto startTime = steady_clock::now();

    sync_wait(then(when_all(then(schedule_after(scheduler, 100ms),
                                 [=]()
                                 {
                                     auto time = steady_clock::now() - startTime;
                                     auto timeMs = duration_cast<milliseconds>(time).count();
                                     std::cout << "part1 finished - [" << timeMs << "]\n";
                                     return time;
                                 }),
                            then(schedule_after(scheduler, 200ms),
                                 [=]()
                                 {
                                     auto time = steady_clock::now() - startTime;
                                     auto timeMs = duration_cast<milliseconds>(time).count();
                                     std::cout << "part2 finished - [" << timeMs << "]\n";
                                     return time;
                                 })),
                   [](auto &&a, auto &&b)
                   {
                       std::cout << "when_all finished - ["
                                 << duration_cast<milliseconds>(std::get<0>(std::get<0>(a))).count()
                                 << ", "
                                 << duration_cast<milliseconds>(std::get<0>(std::get<0>(b))).count()
                                 << "]\n";
                   }));

    std::cout << "all done\n";
}

void simple_let_value()
{
    timed_single_thread_context context;
    auto context_scheduler = context.get_scheduler();

    auto simple_let = let_value(
        then(schedule_after(context_scheduler, 100ms), []()
             { return 42; }),
        [&](int &x)
        {
            return then(
                schedule(context_scheduler),
                [&]() -> int
                { return x + 1; });
        });

    std::optional<int> result = sync_wait(std::move(simple_let));

    std::cout << "result: " << *result << std::endl;
}

void pipeable_let_value()
{
    timed_single_thread_context context;
    auto context_scheduler = context.get_scheduler();

    std::optional<int> result = then(
                                    schedule_after(context_scheduler, 100ms),
                                    []()
                                    { return 42; }) |
                                let_value([&](int &x)
                                          { return then(schedule_after(context_scheduler, 100ms), [&]()
                                                        { return x + 1; }); }) |
                                sync_wait();

    std::cout << "result: " << *result << std::endl;
}

void simple_stop_when()
{
    timed_single_thread_context ctx;

    bool sourceExecuted = false;
    bool triggerExecuted = false;

    std::optional<int> result = sync_wait(
        on(
            ctx.get_scheduler(),
            stop_when(
                then(
                    schedule_after(10ms),
                    [&]
                    {
                        sourceExecuted = true;
                        return 42;
                    }),
                then(
                    schedule_after(1s),
                    [&]
                    { triggerExecuted = true; }))));

    std::cout << "result: " << *result << std::endl;
}

#include <atomic>
#include <unifex/repeat_effect_until.hpp>

void simple_repeat_effect()
{
    timed_single_thread_context context;
    auto scheduler = context.get_scheduler();

    std::atomic<int> count = 0;

    sequence(
        schedule_after(scheduler, 100ms),
        just_from([&]
                  { ++count; std::cout << "count: " << count << std::endl; })) |
        repeat_effect() | stop_when(schedule_after(scheduler, 1s)) |
        sync_wait();

    std::cout << "count: " << count << std::endl;
}

#include <unifex/via.hpp>
#include <unifex/inline_scheduler.hpp>

void simple_via()
{
    inline_scheduler scheduler;

    sync_wait(
        via(
            scheduler,
            then(
                schedule(scheduler),
                []()
                { std::cout << "hello world" << std::endl; })));

    std::cout << "done" << std::endl;
}

task<void> hello_world()
{
    std::cout << "Hello, world!" << std::endl;
    co_return;
}

void simple_task()
{
    timed_single_thread_context context;
    auto scheduler = context.get_scheduler();

    hello_world() | sync_wait();
}

void simple_file_async()
{
    auto load_file_async = [](const std::string &name) -> task<std::string>
    {
        std::ifstream file(name);
        std::string str;

        file.seekg(0, std::ios::end);
        str.reserve(file.tellg());
        file.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());

        co_return str;
    };

    std::string file_name = "test.txt";

    std::optional<std::string> file_contents = load_file_async(file_name) | sync_wait();

    std::cout << "file contents: " << std::endl;
    std::cout << *file_contents << std::endl;
}

int main()
{
    simple_file_async();
    return 0;
}
