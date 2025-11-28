#include "worker.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>

std::vector<std::string> read_commands_from_file(const std::string &filename) {
    std::ifstream ifs(filename);
    if (!ifs) throw std::runtime_error("Cannot open " + filename);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(ifs, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

RunResult execute_commands(SafeFields &sf, const std::vector<std::string> &cmds) {
    using namespace std::chrono;
    // start timing now: file reading excluded by caller
    auto t0 = high_resolution_clock::now();

    for (const auto &line : cmds) {
        std::istringstream iss(line);
        std::string op;
        iss >> op;
        if (op == "read") {
            size_t idx; iss >> idx;
            volatile int val = sf.get(idx); // volatile prevents compiler optimizing away
            (void)val;
        } else if (op == "write") {
            size_t idx; int v; iss >> idx >> v;
            sf.set(idx, v);
        } else if (op == "string") {
            std::string s = static_cast<std::string>(sf);
            (void)s;
        } else {
            // ignore invalid
        }
    }

    auto t1 = high_resolution_clock::now();
    double ms = duration_cast<duration<double, std::milli>>(t1 - t0).count();
    return { ms };
}