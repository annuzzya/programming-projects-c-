#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <syncstream>
#include <thread>

using namespace std::chrono_literals;

void compute(const std::string& name, std::chrono::seconds duration) {
    std::this_thread::sleep_for(duration);
    std::osyncstream(std::cout) << name << '\n';
}

void slow(const std::string& name)  { compute(name, 7s); }
void quick(const std::string& name) { compute(name, 1s); }

void work() {
    using clock = std::chrono::steady_clock;

    auto t0 = clock::now();

    auto fut_C2 = std::async(std::launch::async, [](){
        slow("C2");
        return 0;
    });

    auto fut_B2B3 = std::async(std::launch::async, [](){
        quick("B2");
        quick("B3");
        return 0;
    });

    quick("A");
    slow("B1");

    fut_B2B3.get();
    quick("C1");

    fut_C2.get();
    quick("D");

    auto t1 = clock::now();
    auto elapsed = std::chrono::duration<double>(t1 - t0).count();

    std::osyncstream(std::cout) << "Elapsed (s): " << elapsed << '\n';
    std::osyncstream(std::cout) << "Work is done!" << '\n';
}

int main() {
    work();
    return 0;
}