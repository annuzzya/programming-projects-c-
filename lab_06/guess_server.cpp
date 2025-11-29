#include "guess_server.h"
#include <random>
#include <iostream>

GuessServer play_guess_server(const std::string &) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);

    int secret = dist(gen);

    std::cout << "Server coroutine: secret number chosen!\n";

    while (true) {
        int q = co_await 0;

        if (q < secret) {
            co_yield -1;
        } else if (q > secret) {
            co_yield 1;
        } else {
            co_return 0;
        }
    }
}
