// variant8.cpp
// C++20, hardcoded schedule for variant 8

#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <latch>
#include <syncstream>
#include <chrono>

namespace variant8
{
    struct Scheme
    {
        std::map<char, std::unique_ptr<std::latch>> latch_for;
        std::map<char, std::vector<char>> successors;

        Scheme()
        {
            const std::map<char, int> pred_count{
                    {'a', 0}, {'b', 0}, {'c', 1}, {'d', 2}, {'e', 1},
                    {'f', 1}, {'g', 1}, {'h', 2}, {'i', 1}, {'j', 4}
            };

            for (const auto &p : pred_count)
                latch_for[p.first] = std::make_unique<std::latch>(p.second);

            successors = {
                {'a', {'c', 'd'}},
                {'b', {'d', 'e'}},
                {'c', {'f'}},
                {'d', {'g', 'h'}},
                {'e', {'h', 'i'}},
                {'f', {'j'}},
                {'g', {'j'}},
                {'h', {'j'}},
                {'i', {'j'}},
                {'j', {}}
            };
        }

        void notify_completed(char from)
        {
            for (char s : successors[from])
                latch_for[s]->count_down();
        }

        void wait_for_predecessors(char s)
        {
            latch_for[s]->wait();
        }
    };

    inline Scheme scheme;
}
