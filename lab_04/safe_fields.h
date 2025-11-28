#ifndef SAFE_FIELDS_H
#define SAFE_FIELDS_H

#include <vector>
#include <shared_mutex>
#include <string>

class SafeFields {
public:
    explicit SafeFields(size_t m = 3);

    SafeFields(const SafeFields &) = delete;

    SafeFields &operator=(const SafeFields &) = delete;

    ~SafeFields() = default;

    int get(size_t idx) const;

    void set(size_t idx, int value);

    operator std::string() const;

private:
    std::vector<int> fields;
    mutable std::vector<std::shared_mutex> mutexes;
};
#endif
