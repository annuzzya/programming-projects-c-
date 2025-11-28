#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <chrono>
#include <iomanip>

#include "safe_fields.h"

using namespace std;

vector<string> read_cmd_file(const string &filename) {
    ifstream f(filename);
    if (!f) throw runtime_error("Cannot open file: " + filename);

    vector<string> cmds;
    string line;
    while (getline(f, line)) {
        if (!line.empty()) cmds.push_back(line);
    }
    return cmds;
}

double run_commands(SafeFields &sf, const vector<string> &cmds) {
    using namespace chrono;

    auto start = high_resolution_clock::now();

    for (const auto &line: cmds) {
        istringstream iss(line);
        string op;
        iss >> op;

        if (op == "read") {
            size_t idx;
            iss >> idx;
            volatile int v = sf.get(idx);
            (void) v;
        } else if (op == "write") {
            size_t idx;
            int val;
            iss >> idx >> val;
            sf.set(idx, val);
        } else if (op == "string") {
            string s = (string) sf;
            (void) s;
        }
    }

    auto end = high_resolution_clock::now();
    return duration_cast<duration<double, milli> >(end - start).count();
}

double run_multithread(const vector<string> &filenames) {
    SafeFields sf(3);

    vector<vector<string> > cmds;
    for (auto &file: filenames)
        cmds.push_back(read_cmd_file(file));

    vector<double> times(filenames.size(), 0.0);
    vector<thread> th;

    for (int i = 0; i < filenames.size(); ++i) {
        th.emplace_back([&sf, &cmds, &times, i]() {
            times[i] = run_commands(sf, cmds[i]);
        });
    }
    for (auto &t: th) t.join();

    return *max_element(times.begin(), times.end());
}

int main() {
    cout << fixed << setprecision(3);

    cout << "Running 1 thread...\n";
    double t1 = run_multithread({"input_thread1.txt"});
    cout << "Time: " << t1 << " ms\n\n";

    cout << "Running 2 threads...\n";
    double t2 = run_multithread({"input_thread1.txt", "input_thread2.txt"});
    cout << "Time: " << t2 << " ms\n\n";

    cout << "Running 3 threads...\n";
    double t3 = run_multithread({"input_thread1.txt", "input_thread2.txt", "input_thread3.txt"});
    cout << "Time: " << t3 << " ms\n\n";

    cout << "==== SUMMARY ====\n";
    cout << "1 thread: " << t1 << " ms\n";
    cout << "2 threads: " << t2 << " ms\n";
    cout << "3 threads: " << t3 << " ms\n";

    return 0;
}
