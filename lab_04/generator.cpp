#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <cassert>

using Cmd = std::string;

// generate sequence of commands according to given percentages for m=3
// percentages vector must have length 7: read0, write0, read1, write1, read2, write2, string
void generate_file(const std::string &filename, size_t n_commands, const std::vector<int> &percents, unsigned seed = 0) {
    assert(percents.size() == 7);
    std::mt19937_64 eng(seed ? seed : std::random_device{}());
    std::discrete_distribution<int> dist(percents.begin(), percents.end());

    std::ofstream ofs(filename);
    if (!ofs) throw std::runtime_error("Can't open " + filename);

    for (size_t i = 0; i < n_commands; ++i) {
        int pick = dist(eng);
        switch (pick) {
            case 0: ofs << "read 0\n"; break;
            case 1: ofs << "write 0 1\n"; break;
            case 2: ofs << "read 1\n"; break;
            case 3: ofs << "write 1 1\n"; break;
            case 4: ofs << "read 2\n"; break;
            case 5: ofs << "write 2 1\n"; break;
            case 6: ofs << "string\n"; break;
        }
    }
}

// helper to generate three modes:
// mode "variant" => percents as provided
// mode "equal" => distribute evenly across operations (7 ops)
// mode "bad" => strongly skewed manual distribution
void generate_three_modes(const std::string &base_name, size_t n_commands, const std::vector<int> &variant_percents) {
    // variant
    generate_file(base_name + "_variant.txt", n_commands, variant_percents, 1234);

    // equal
    std::vector<int> equal(7, 1); // uniform
    generate_file(base_name + "_equal.txt", n_commands, equal, 4321);

    // bad skewed (manual)
    // We'll make lots of writes to field 0, almost no reads, many strings (to stress operator)
    std::vector<int> bad = {5, 40, 1, 1, 1, 1, 51}; // sums matter not (discrete handles)
    generate_file(base_name + "_bad.txt", n_commands, bad, 2025);
}
