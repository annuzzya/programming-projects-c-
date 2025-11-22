#ifndef LAB2_REPLACE_EXPERIMENTS_H
#define LAB2_REPLACE_EXPERIMENTS_H

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <execution>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>

using clk = std::chrono::steady_clock;
using microseconds = std::chrono::microseconds;

struct BenchResult {
    std::string label;
    long long time_us = 0;
    double throughput_melems_per_s = 0.0;
    std::size_t replaced_count = 0;
};

void print_env_header();

void print_result_row(const BenchResult &r);

void run_experiments();

template<class Pred>
std::size_t count_replaced(const std::vector<int> &v, Pred pred, int repl);

template<class Pred>
BenchResult bench_replace_if_no_policy(const std::vector<int> &src, Pred pred, int replace_with);

template<class ExecPolicy, class Pred>
BenchResult bench_replace_if_policy(const std::string &name, ExecPolicy pol, const std::vector<int> &src, Pred pred,
                                    int replace_with);

template<class Pred>
BenchResult bench_custom_parallel(const std::vector<int> &src, Pred pred, int replace_with, int K);

#include "replace_experiments.tpp"

#endif
