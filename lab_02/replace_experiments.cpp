#include "replace_experiments.h"
#include <limits>

using namespace std;

void print_result_row(const BenchResult &r) {
    cout << left << setw(40) << r.label
            << right << setw(10) << r.time_us << " us"
            << "   " << fixed << setprecision(2)
            << setw(8) << r.throughput_melems_per_s << " Me/s"
            << "   (replaced≈" << r.replaced_count << ")\n";
}

void run_experiments() {
    const vector<size_t> SIZES = {100'000, 1'000'000, 5'000'000};
    const int SEQ_VARIANTS = 3;
    const int RANGE_MAX = 1'000'000;
    const int THRESHOLD = RANGE_MAX / 10;
    const int REPL = -1;
    auto pred = [=](int x) noexcept { return x < THRESHOLD; };

    unsigned hc = thread::hardware_concurrency();
    const int K_MIN = 1;
    const int K_MAX = max(2u, hc * 2u);

    cout << "Predicate: x < " << THRESHOLD
            << "  (values in [0," << RANGE_MAX << "])   replacement=" << REPL << "\n";
    cout << "------------------------------------------------------------\n";

    for (size_t N: SIZES) {
        cout << "Data size N = " << N << " elements \n";

        for (int s = 0; s < SEQ_VARIANTS; ++s) {
            const uint64_t seed = 0xC0FFEEULL ^ (static_cast<uint64_t>(N) * 1315423911ULL) ^ static_cast<uint64_t>(s);
            mt19937_64 gen(seed);
            uniform_int_distribution<int> dist(0, RANGE_MAX);
            vector<int> data(N);
            for (size_t i = 0; i < N; ++i) data[i] = dist(gen);
            {
                vector<int> tmp;
                tmp.assign(data.begin(), data.begin() + min<std::size_t>(data.size(), 1024));
                replace_if(tmp.begin(), tmp.end(), pred, REPL);
            }

            cout << "Sequence variant: " << (s + 1) << " \n";

            auto r0 = bench_replace_if_no_policy(data, pred, REPL);
            auto r1 = bench_replace_if_policy("std::replace_if (seq)", execution::seq, data, pred, REPL);
            auto r2 = bench_replace_if_policy("std::replace_if (par)", execution::par, data, pred, REPL);
            auto r3 = bench_replace_if_policy("std::replace_if (par_unseq)", execution::par_unseq, data, pred, REPL);

            print_result_row(r0);
            print_result_row(r1);
            print_result_row(r2);
            print_result_row(r3);

            cout << "------------------------------------------------------------\n";
            cout << "Custom parallel reduce-with-merge (per K):\n";
            cout << left << setw(8) << "K"
                    << right << setw(14) << "time (us)"
                    << setw(20) << "throughput (Me/s)"
                    << setw(18) << "replaced≈count\n";

            long long best_time = numeric_limits<long long>::max();
            int best_K = 1;

            for (int K = K_MIN; K <= K_MAX; ++K) {
                auto r = bench_custom_parallel(data, pred, REPL, K);
                cout << left << setw(8) << K
                        << right << setw(14) << r.time_us
                        << setw(20) << fixed << setprecision(2) << r.throughput_melems_per_s
                        << setw(18) << r.replaced_count << "\n";

                if (r.time_us < best_time) {
                    best_time = r.time_us;
                    best_K = K;
                }
            }

            cout << "Best K for this sequence: " << best_K
                    << "  (time=" << best_time << " us)"
                    << "  vs hardware threads=" << (hc ? hc : 1)
                    << "  → ratio K/hw=" << fixed << setprecision(2)
                    << (hc ? (static_cast<double>(best_K) / hc) : static_cast<double>(best_K)) << "\n";
        }
    }
}
