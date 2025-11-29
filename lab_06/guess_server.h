#ifndef LAB_04_GUESS_SERVER_H
#define LAB_04_GUESS_SERVER_H

#include <coroutine>
#include <stdexcept>
#include <string>

class GuessServer {
public:
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;

    bool in_progress() const { return coro && !coro.done(); }

    bool move_next() {
        return in_progress() ? (coro.resume(), !coro.done()) : false;
    }

    int current_value_safety() const {
        if (!coro) throw std::runtime_error("coroutine is destroyed");
        return coro.promise().result;
    }

    void set_request(int r) {
        if (!coro) throw std::runtime_error("coroutine is destroyed");
        coro.promise().request = r;
    }

    operator bool() const noexcept { return static_cast<bool>(coro); }

    class Awaiter {
    public:
        explicit Awaiter(promise_type &p) : p(p) {
        }

        bool await_ready() const noexcept { return true; }
        bool await_suspend(std::coroutine_handle<>) noexcept { return false; }
        int await_resume() noexcept { return p.request; }

    private:
        promise_type &p;
    };

    struct promise_type {
        int result = 0;
        int request = 0;

        void unhandled_exception() { std::terminate(); }
        auto initial_suspend() { return std::suspend_never{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }

        auto yield_value(int value) {
            result = value;
            return std::suspend_always{};
        }

        void return_value(int value) {
            result = value;
        }

        auto get_return_object() {
            return GuessServer{handle::from_promise(*this)};
        }

        Awaiter await_transform(int) { return Awaiter{*this}; }
    };

    GuessServer(GuessServer const &) = delete;

    GuessServer &operator=(GuessServer const &) = delete;

    GuessServer(GuessServer &&other) noexcept : coro(other.coro) { other.coro = nullptr; }

    GuessServer &operator=(GuessServer &&other) noexcept {
        if (this != &other) {
            if (coro) coro.destroy();
            coro = other.coro;
            other.coro = nullptr;
        }
        return *this;
    }

    ~GuessServer() {
        if (coro) coro.destroy();
    }

private:
    explicit GuessServer(handle h) : coro(h) {
    }

    handle coro = nullptr;
};

GuessServer play_guess_server(const std::string &seed);

#endif //LAB_04_GUESS_SERVER_H
