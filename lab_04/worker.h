#ifndef LAB_03_WORKER_H
#define LAB_03_WORKER_H
#pragma once
#include "safe_fields.h"
#include <string>
#include <vector>
#include <chrono>

struct RunResult {
    double elapsed_ms;
};

std::vector<std::string> read_commands_from_file(const std::string &filename);
RunResult execute_commands(SafeFields &sf, const std::vector<std::string> &cmds);

#endif //LAB_03_WORKER_H