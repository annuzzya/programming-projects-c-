#pragma once
#include <vector>
#include <shared_mutex>
#include <string>

class SafeFields {
public:
    explicit SafeFields(size_t m = 3);

    SafeFields(const SafeFields&) = delete;
    SafeFields& operator=(const SafeFields&) = delete;
    ~SafeFields() = default;

    int get(size_t idx) const;       // read
    void set(size_t idx, int value); // write
    operator std::string() const;    // convert to string

    size_t size() const noexcept { return fields.size(); }

private:
    std::vector<int> fields;
    mutable std::vector<std::shared_mutex> mutexes;
};
