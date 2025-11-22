#pragma once

#include "replace_experiments.h"

template<class Pred>
std::size_t count_replaced(const std::vector<int> &v, Pred pred, int repl) {
    return static_cast<std::size_t>(std::count(v.begin(), v.end(), repl));
}

template<class Pred>
BenchResult bench_replace_if_no_policy(const std::vector<int> &src, Pred pred, int replace_with) {
    std::vector<int> w = src;
    auto t0 = clk::now();
    std::replace_if(w.begin(), w.end(), pred, replace_with);
    auto t1 = clk::now();
    long long us = std::chrono::duration_cast<microseconds>(t1 - t0).count();

    BenchResult r;
    r.label = "std::replace_if (no policy)";
    r.time_us = us;
    r.throughput_melems_per_s = (us > 0) ? static_cast<double>(w.size()) / static_cast<double>(us) : 0.0;
    r.replaced_count = count_replaced(w, pred, replace_with);
    return r;
}

template<class ExecPolicy, class Pred>
BenchResult bench_replace_if_policy(const std::string &name, ExecPolicy pol, const std::vector<int> &src, Pred pred,
                                    int replace_with) {
    std::vector<int> w = src;
    auto t0 = clk::now();
    std::replace_if(pol, w.begin(), w.end(), pred, replace_with);
    auto t1 = clk::now();
    long long us = std::chrono::duration_cast<microseconds>(t1 - t0).count();

    BenchResult r;
    r.label = name;
    r.time_us = us;
    r.throughput_melems_per_s = (us > 0) ? static_cast<double>(w.size()) / static_cast<double>(us) : 0.0;
    r.replaced_count = count_replaced(w, pred, replace_with);
    return r;
}

template<class Pred>
BenchResult bench_custom_parallel(const std::vector<int> &src, Pred pred,
                                  int replace_with, int K) {
    const std::size_t N = src.size();
    const std::size_t base = N / static_cast<std::size_t>(K);
    const std::size_t rem = N % static_cast<std::size_t>(K);

    std::vector<std::pair<std::size_t, std::size_t> > ranges;
    ranges.reserve(K);
    std::size_t off = 0;
    for (int i = 0; i < K; ++i) {
        std::size_t len = base + (i < static_cast<int>(rem) ? 1u : 0u);
        ranges.emplace_back(off, off + len);
        off += len;
    }

    auto t0 = clk::now();
    std::vector<std::vector<int> > buffers(K);
    std::vector<std::jthread> threads;
    threads.reserve(K);

    for (int i = 0; i < K; ++i) {
        threads.emplace_back([&, i] {
            auto [b, e] = ranges[i];
            buffers[i].assign(src.begin() + b, src.begin() + e);
            std::replace_if(std::execution::seq, buffers[i].begin(), buffers[i].end(), pred, replace_with);
        });
    }

    std::vector<int> result(N);
    std::size_t pos = 0;
    for (int i = 0; i < K; ++i) {
        auto &buf = buffers[i];
        result.insert(result.end(), buf.begin(), buf.end());
    }

    auto t1 = clk::now();
    long long us = std::chrono::duration_cast<microseconds>(t1 - t0).count();

    BenchResult r;
    r.label = "Custom parallel (K=" + std::to_string(K) + ")";
    r.time_us = us;
    r.throughput_melems_per_s = (us > 0) ? (1e-6 * N / (us * 1e-6)) : 0.0;
    r.replaced_count = count_replaced(result, pred, replace_with);
    return r;
}
