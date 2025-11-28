#include <iostream>
#include <vector>
#include <thread>
#include <iomanip>
#include <string>

#include "safe_fields.h"
#include "worker.h"

// Генератор команд
void generate_file(const std::string &filename, size_t n_commands,
                   const std::vector<int> &percents, unsigned seed = 0);
void generate_three_modes(const std::string &base_name, size_t n_commands,
                          const std::vector<int> &variant_percents);

// Запуск одного сценарію: filenames.size() == кількості потоків
double run_scenario(const std::vector<std::string> &filenames) {
    using namespace std;

    // Читаємо файли заздалегідь (час читання не рахуємо)
    vector<vector<string>> all_cmds;
    for (auto &f : filenames) {
        all_cmds.push_back(read_commands_from_file(f));
    }

    SafeFields sf(3); // 3 поля
    vector<thread> threads;
    vector<double> times(filenames.size(), 0.0);

    // Запускаємо виконання
    for (size_t i = 0; i < filenames.size(); ++i) {
        threads.emplace_back([&sf, &cmds = all_cmds[i], &times, i]() {
            RunResult r = execute_commands(sf, cmds);
            times[i] = r.elapsed_ms;
        });
    }

    for (auto &t : threads) t.join();

    // Загальний час — максимальний серед потоків (консервативний підхід)
    double mx = *max_element(times.begin(), times.end());
    return mx;
}

int main() {
    using namespace std;

    cout << fixed << setprecision(3);

    // Частоти для варіанта №8:
    // read0=20%, write0=10%, read1=25%, write1=10%, read2=20%, write2=10%, string=5%
    vector<int> variant = {20, 10, 25, 10, 20, 10, 5};

    size_t n_commands = 200000; // можна збільшити при потребі
    cout << "Generating command files..." << endl;

    try {
        generate_three_modes("thread", n_commands, variant);
    } catch (exception &e) {
        cerr << "Generator error: " << e.what() << endl;
        return 1;
    }

    vector<string> modes = {"variant", "equal", "bad"};

    // Створюємо окремі файли для кожного потоку (t0, t1, t2)
    for (auto &mode : modes) {
        for (int t = 0; t < 3; ++t) {
            string name = "thread_" + mode + "_t" + to_string(t) + ".txt";
            if (mode == "variant") {
                generate_file(name, n_commands / 3, variant, 100 + t);
            } else if (mode == "equal") {
                generate_file(name, n_commands / 3, vector<int>(7, 1), 200 + t);
            } else {
                generate_file(name, n_commands / 3,
                              vector<int>{5, 40, 1, 1, 1, 1, 51}, 300 + t);
            }
        }
    }

    // Таблиця 3×3
    vector<vector<double>> table(3, vector<double>(3, 0.0));

    cout << "\nRunning experiments...\n";

    for (int threads = 1; threads <= 3; ++threads) {
        for (int mi = 0; mi < (int)modes.size(); ++mi) {
            string mode = modes[mi];
            vector<string> files;

            for (int t = 0; t < threads; ++t) {
                files.push_back("thread_" + mode + "_t" + to_string(t) + ".txt");
            }

            cout << "Threads = " << threads << ", Mode = " << mode << endl;
            double time_ms = run_scenario(files);
            cout << "    Time: " << time_ms << " ms\n";

            table[threads - 1][mi] = time_ms;
        }
    }

    cout << "\n==================== 3×3 RESULT TABLE ====================\n";
    cout << "Rows = threads (1, 2, 3)\n";
    cout << "Cols = variant | equal | bad\n\n";

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            cout << setw(12) << table[r][c];
        }
        cout << "\n";
    }

    cout << "\nDone.\n";

    return 0;
}
